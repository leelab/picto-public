#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "SignalDataIterator.h"
#include "../storage/SignalChannelInfo.h"
using namespace Picto;

SignalDataIterator::SignalDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,QString signalName)
: AnalysisDataIterator(qsEngine,session)
{
	getSubChanInfo(signalName);
	numSubChans_ = 0;
	approxValsPerRow_ = 1;

	getSubChanInfo(signalName);
}

SignalDataIterator::~SignalDataIterator()
{

}

QString SignalDataIterator::propertyDescriptor()
{
	return "SigTable:"+tableName_;
}

bool SignalDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	if(!isValid())
		return false;
	//Get signal value list.
	//NOTE: In the case of all values in the picto session that reference a frameid EXCEPT for signalchannels,
	//The frame reference is the frame on which the value took effect (ie. Visible=true may have been set in 
	//picto at some time during the presentation of frame 1, but it only affected the user when the object actually
	//became visible sometime during frame 2, so it is marked with frame 2).
	//In the case of signal channels however, the data is an input into the system.  The system, and the operator
	//care about what the user was responding to and so signal channel readings are marked with the frame that
	//was displayed just before they occured.  The offsettime value of the signal channel is therefore an offset
	//from the time of the channel's marked frame id.  It is the offset in time from when the frame marked frame
	//id occured to the time of the first signal channel reading.
	QString queryString = QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"s.dataid > :lastdataid AND f.time <= :stoptime ORDER BY s.dataid LIMIT :maxrows").arg(tableName_);
	query->prepare(queryString);
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",stopTime);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool SignalDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	if(!isValid())
		return false;
	//Get signal value list.
	//NOTE: In the case of all values in the picto session that reference a frameid EXCEPT for signalchannels,
	//The frame reference is the frame on which the value took effect (ie. Visible=true may have been set in 
	//picto at some time during the presentation of frame 1, but it only affected the user when the object actually
	//became visible sometime during frame 2, so it is marked with frame 2).
	//In the case of signal channels however, the data is an input into the system.  The system, and the operator
	//care about what the user was responding to and so signal channel readings are marked with the frame that
	//was displayed just before they occured.  The offsettime value of the signal channel is therefore an offset
	//from the time of the channel's marked frame id.  It is the offset in time from when the frame marked frame
	//id occured to the time of the first signal channel reading.
	QString queryString = QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"f.time < :beforetime ORDER BY s.dataid DESC LIMIT 1").arg(tableName_);
	query->prepare(queryString);
	query->bindValue(":beforetime",beforeTime);
	return true;
}

void SignalDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	if(tableName_.isEmpty())
		query->prepare(QString("SELECT 0"));
	else
		query->prepare(QString("SELECT COUNT(dataid) FROM %1").arg(tableName_));
}

qulonglong SignalDataIterator::readOutRecordData(QSqlRecord* record)
{
	//Convert data from blob to float array.  Get the relevant subchannel
	//and calculate its time.
	QByteArray dataByteArray = record->value(3).toByteArray();
	int numEntries = dataByteArray.size()/sizeof(float);
	if(approxValsPerRow_ == 0)
		approxValsPerRow_ = numEntries;
	float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
	for(int i=0;i<numEntries;i+=numSubChans_)
	{
		//Don't enter the data id.  Since the individual readings don't have data ids, but
		//do have different times, we want to use time as the index to be sure that it will
		//be possible to use eye readings as a trigger.
		//(ie. If we copied the dataid into the SignalData structs of each of a single row's
		//sub entries and we then used the signal as a trigger and a data source, we would 
		//end up missing signal points in the final readout.)
		QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(1).toDouble()+record->value(2).toDouble()+((i/numSubChans_)*samplePeriod_)));
		val->index.idSource_ = EventOrderIndex::BEHAVIORAL;
		QScriptValue dimensionArray = createScriptArray(numSubChans_);
		for(int j=0;j<numSubChans_;j++)
		{
			dimensionArray.setProperty(j,floatArray[i+j]);
		}
		val->scriptVal.setProperty("value",dimensionArray);
		val->scriptVal.setProperty("time",val->index.time_);
	}
	return record->value(0).toLongLong();
}

void SignalDataIterator::getSubChanInfo(QString signalName)
{
	numSubChans_ = -1;
	QSqlQuery query = getSessionQuery();
	query.setForwardOnly(true);
	query.prepare("SELECT value FROM sessioninfo WHERE key='Signal'");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
		return;
	}
	if(!query.next())
	{
		return;
	}
	do
	{
		SignalChannelInfo sigInf;
		sigInf.fromXml(query.value(0).toString());
		if(sigInf.getName().toLower() != signalName.toLower())
			continue;
		tableName_ = sigInf.getTableName();
		numSubChans_ = sigInf.getSubchannels();
		subChanNames_ = sigInf.getSubchannelNames().split(",",QString::SkipEmptyParts);
		samplePeriod_ = double(sigInf.getResolution())/1000.0;
		QString subChanNames = sigInf.getSubchannelNames();
		QStringList subChanNameList = subChanNames.split(",",QString::SkipEmptyParts);
	}while(query.next());
}