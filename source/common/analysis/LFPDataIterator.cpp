#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "../storage/datasourceinfo.h"
#include "../storage/alignmentinfo.h"
#include "LFPDataIterator.h"
using namespace Picto;

LFPDataIterator::LFPDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
: AnalysisDataIterator(qsEngine,session),
	offsetTime_(0),
	temporalFactor_(1.0)
{
	approxValsPerRow_ = 0;
	getSamplePeriod();
}

LFPDataIterator::~LFPDataIterator()
{

}

void LFPDataIterator::updateVariableSessionConstants()
{
	QSqlQuery query = getSessionQuery();
	query.setForwardOnly(true);

	QString queryString = QString("SELECT value FROM sessioninfo WHERE key=\"AlignmentInfo\"");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}
	if(query.next())
	{
		AlignmentInfo inf;
		inf.fromXml(query.value(0).toString());
		offsetTime_ = inf.getOffsetTime();
		temporalFactor_ = inf.getTemporalFactor();
	}
}

bool LFPDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	if(temporalFactor_ == 0)
		temporalFactor_ = 1.0;
	QString queryString = QString("SELECT dataid,timestamp,data,channel "
		"FROM lfp WHERE dataid > :lastDataId AND timestamp <= :stoptime ORDER BY dataid LIMIT :maxrows");
	query->prepare(queryString);
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",(stopTime-offsetTime_)/temporalFactor_);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool LFPDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	QString queryString = QString("SELECT dataid,timestamp,data,channel "
		"FROM lfp WHERE timestamp = "
			"(SELECT timestamp FROM lfp WHERE timestamp < :beforetime ORDER BY dataid DESC LIMIT 1)"
			);
	query->prepare(queryString);
	query->bindValue(":beforetime",(beforeTime-offsetTime_)/temporalFactor_);
	return true;
}

void LFPDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(dataid) FROM lfp");
}

qulonglong LFPDataIterator::readOutRecordData(QSqlRecord* record)
{
	if(approxValsPerRow_ == 0)
	{
		QByteArray dataByteArray = record->value(2).toByteArray();
		approxValsPerRow_ = dataByteArray.size()/sizeof(float);
	}
	double neuralStartTime = record->value(1).toDouble();
	double startTime = offsetTime_+temporalFactor_*neuralStartTime;
	unsigned int chan = record->value(3).toInt();
	Q_ASSERT((temporalFactor_ > 0) && (samplePeriod_ > 0));
	//Size latestRecords_ list so that this channel has a spot.
	while(timesByChannel_.size() <= chan)
		timesByChannel_.append(-1.0E100);
	//Get the previous timestamp for this channel
	double lastTimeForChan = timesByChannel_[chan];
	timesByChannel_[chan] = startTime;
	//Append this record to the time sorted map
	timeSortedRecords_[startTime][chan]=QSqlRecord(*record);
	//In a live recording, If a channel had data with a timestamp earlier than lastTimeForChan
	//and it didn't arrive yet, it would mean that either the lfp recording 
	//system skipped that channel's data for some reason (which we assume 
	//won't occur).  That that channel is no longer collecting data. Or that that channel's 
	//data packet was delivered late to the server due to connectivity issues.  
	//We aren't going to worry about connectivity issues since the data will all be at 
	//the server eventually and our live analysis is not too important.  For that reason,
	//we go through the timeSortedRecords_ at this point, remove all records that have
	//timestamp before lastTimeForChan and generate analysis values with them.
	outputEverythingBefore(lastTimeForChan);
	return record->value(0).toLongLong();
}

void LFPDataIterator::dataFinishedWithSessionEnded()
{
	if(!timeSortedRecords_.size())
		return;
	double lastTime = (timeSortedRecords_.end()-1).key();
	outputEverythingBefore(lastTime+1);
}

void LFPDataIterator::outputEverythingBefore(double beforeTime)
{
	QMap<double,QMap<unsigned int,QSqlRecord>>::iterator outIter = timeSortedRecords_.begin();
	QMap<unsigned int,QSqlRecord>::iterator inIter;
	while((outIter.key() < beforeTime) && (outIter != timeSortedRecords_.end()))
	{
		//We want to add the channel in order so that the stable sort
		//will put them in order in the output AnalysisValues.
		inIter = outIter.value().begin();
		while(inIter != outIter.value().end())
		{
			addToOutputData(inIter.value());
			inIter = outIter.value().erase(inIter);
		}
		outIter = timeSortedRecords_.erase(outIter);
	}
	//Sort the data and create AnalysisValues with it
	sortOutputDataAndCreateAnalysisValues();
}

void LFPDataIterator::addToOutputData(QSqlRecord record)
{
	//Convert data from blob to float array.  Get the relevant subchannel
	//and calculate its time.
	double startTime = offsetTime_+temporalFactor_*record.value(1).toDouble();
	double increment = temporalFactor_*samplePeriod_;
	unsigned int chan = record.value(3).toInt();
	QByteArray dataByteArray = record.value(2).toByteArray();
	int numEntries = dataByteArray.size()/sizeof(float);
	float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
	int startingSize = sortedData_.size();
	sortedData_.resize(startingSize+numEntries);
	double currTime;
	for(int i=0;i<numEntries;i++)
	{
		currTime = startTime + i*increment;
		sortedData_[startingSize+i] = lfpData(currTime,chan,floatArray[i]);
	}
}

void LFPDataIterator::sortOutputDataAndCreateAnalysisValues()
{
	qStableSort(sortedData_.begin(),sortedData_.end());
	QSharedPointer<AnalysisValue> val;
	for(int i=0;i<sortedData_.size();i++)
	{
		val = createNextAnalysisValue(EventOrderIndex(sortedData_[i].time));
		val->index.idSource_ = EventOrderIndex::NEURAL;
		val->scriptVal.setProperty("time",sortedData_[i].time);
		val->scriptVal.setProperty("value",sortedData_[i].value);
		val->scriptVal.setProperty("channel",sortedData_[i].channel);
	}
	sortedData_.clear();
}

void LFPDataIterator::getSamplePeriod()
{
	samplePeriod_ = 0;
	QSqlQuery query = getSessionQuery();
	query.setForwardOnly(true);

	QString queryString = QString("SELECT value FROM sessioninfo WHERE key=\"DataSource\"");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}
	while(query.next())
	{
		DataSourceInfo src;
		src.fromXml(query.value(0).toString());
		if(src.getName() == "lfp")
		{
			samplePeriod_ = src.getResolution();
			break;
		}
	}
}