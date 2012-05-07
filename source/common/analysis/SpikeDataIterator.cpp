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
: AnalysisDataIterator(qsEngine,session)
{
	getSamplePeriod();
}

SpikeDataIterator::~SpikeDataIterator()
{

}

void SpikeDataIterator::updateVariableSessionConstants()
{
	offsetTime_ = 0;
	temporalFactor_ = 0;
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

bool SpikeDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId)
{
	QString queryString = QString("SELECT dataid,timestamp,channel,unit,waveform "
		"FROM spikes WHERE dataid > :lastDataId ORDER BY dataid LIMIT 10000");
	query->prepare(queryString);
	query->bindValue(":lastdataid",lastDataId);
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
	return record->value(0).toLongLong();
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
		qDebug("Query Failed: " + query.lastError().text().toAscii());
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