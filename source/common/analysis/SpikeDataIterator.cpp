#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "../storage/datasourceinfo.h"
#include "../storage/alignmentinfo.h"
#include "SpikeDataIterator.h"
using namespace Picto;

SpikeDataIterator::SpikeDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
: AnalysisDataIterator(qsEngine,session),
	offsetTime_(0),
	temporalFactor_(1.0)
{
	getSamplePeriod();
}

SpikeDataIterator::~SpikeDataIterator()
{

}

void SpikeDataIterator::updateVariableSessionConstants()
{
	QSqlQuery query = getSessionQuery();
	query.setForwardOnly(true);

	QString queryString = QString("SELECT value FROM sessioninfo WHERE key=\"AlignmentInfo\"");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
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

bool SpikeDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	if(temporalFactor_ == 0)
		temporalFactor_ = 1.0;
	QString queryString = QString("SELECT dataid,timestamp,channel,unit,waveform "
		"FROM spikes WHERE dataid > :lastDataId AND timestamp <= :stoptime ORDER BY dataid LIMIT :maxrows");
	query->prepare(queryString);
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",(stopTime-offsetTime_)/temporalFactor_);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool SpikeDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	QString queryString = QString("SELECT dataid,timestamp,channel,unit,waveform "
		"FROM spikes WHERE timestamp < :beforetime ORDER BY dataid DESC LIMIT 1");
	query->prepare(queryString);
	query->bindValue(":beforetime",(beforeTime-offsetTime_)/temporalFactor_);
	return true;
}

void SpikeDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(dataid) FROM spikes");
}

qulonglong SpikeDataIterator::readOutRecordData(QSqlRecord* record)
{
	//Build waveform
	QByteArray dataByteArray = record->value(4).toByteArray();
	int numEntries = dataByteArray.size()/sizeof(float);
	float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
	QStringList wave;
	for(int i=0;i<numEntries;i++)
	{
		wave.append(QString::number(floatArray[i]));
	}

	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(offsetTime_ + temporalFactor_*record->value(1).toDouble(),record->value(0).toLongLong(),EventOrderIndex::NEURAL));
	val->scriptVal.setProperty("channel",record->value(2).toInt());
	val->scriptVal.setProperty("unit",record->value(3).toInt());
	val->scriptVal.setProperty("samplePeriod",temporalFactor_*samplePeriod_);
	val->scriptVal.setProperty("waveSize",numEntries);
	val->scriptVal.setProperty("wave",wave.join(","));
	QScriptValue waveScriptArray = createScriptArray(numEntries);
	for(int i=0;i<numEntries;i++)
	{
		waveScriptArray.setProperty(i,wave[i]);
	}
	val->scriptVal.setProperty("waveValue",waveScriptArray);
	return val->index.dataId_;
}

void SpikeDataIterator::getSamplePeriod()
{
	samplePeriod_ = 0;
	QSqlQuery query = getSessionQuery();
	query.setForwardOnly(true);

	QString queryString = QString("SELECT value FROM sessioninfo WHERE key=\"DataSource\"");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
		return;
	}
	while(query.next())
	{
		DataSourceInfo src;
		src.fromXml(query.value(0).toString());
		if(src.getName() == "spikes")
		{
			samplePeriod_ = src.getResolution();
			break;
		}
	}
}