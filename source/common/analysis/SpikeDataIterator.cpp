#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "../storage/datasourceinfo.h"
#include "../storage/alignmentinfo.h"
#include "SpikeDataIterator.h"
using namespace Picto;

SpikeDataIterator::SpikeDataIterator(QSqlDatabase session)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readQueries_ = 0;
	sessionEnded_ = false;

	getSamplePeriod();
	updateTotalQueryCount();
}

SpikeDataIterator::~SpikeDataIterator()
{

}

QSharedPointer<SpikeData> SpikeDataIterator::getNextSpikeVals()
{
	if(spikeVals_.size())
		return spikeVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updateSpikeValsList();
	if(spikeVals_.size())
		return spikeVals_.takeFirst();
	//No new data, return an empty SpikeData()
	return QSharedPointer<SpikeData>( new SpikeData());
}

void SpikeDataIterator::updateSpikeValsList()
{
	if(!isValid())
		return;
	if(readQueries_ >= totalQueries_)
		return;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	if(!sessionEnded_)
	{
		//Get latest align coefficients
		getAlignCoefficients();

		//Check if session has ended
		query.prepare("SELECT key "
			"FROM sessioninfo WHERE key='sessionend'");
		bool success = query.exec();
		if(!success)
		{
			qDebug("Query Failed: " + query.lastError().text().toAscii());
			return;
		}
		sessionEnded_ = query.next();
	}

	//Get spike value list.
	QString queryString = QString("SELECT dataid,timestamp,channel,unit,waveform "
		"FROM spikes WHERE dataid > :lastDataId ORDER BY dataid LIMIT 10000");
	query.prepare(queryString);
	query.bindValue(":lastdataid",lastSessionDataId_);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	while(query.next()){
		//Build waveform
		QByteArray dataByteArray = query.value(4).toByteArray();
		int numEntries = dataByteArray.size()/sizeof(float);
		float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
		QString wave;
		for(int i=0;i<numEntries;i++)
		{
			wave.append(QString::number(floatArray[i])+",");
		}
		if(numEntries)
			wave.resize(wave.size()-1);//Get rid of last comma

		//Add data to list
		spikeVals_.append(QSharedPointer<SpikeData>(new SpikeData(query.value(0).toLongLong()
							,offsetTime_ + temporalFactor_*query.value(1).toDouble()
							,query.value(2).toInt()
							,query.value(3).toInt(),
							temporalFactor_*samplePeriod_,numEntries,wave)) );
		if(query.value(0).toLongLong() > lastSessionDataId_)
			lastSessionDataId_ = query.value(0).toLongLong();
		readQueries_++;
	}
	if(readQueries_ > totalQueries_)
		updateTotalQueryCount();
}

void SpikeDataIterator::getSamplePeriod()
{
	samplePeriod_ = 0;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
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

void SpikeDataIterator::getAlignCoefficients()
{
	offsetTime_ = 0;
	temporalFactor_ = 0;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
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

void SpikeDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT COUNT(dataid) FROM spikes");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	if(query.next())
		totalQueries_ = query.value(0).toInt();
}