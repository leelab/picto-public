#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "SpikeDataIterator.h"
using namespace Picto;

SpikeDataIterator::SpikeDataIterator(QSqlDatabase session)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readQueries_ = 0;

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

	//Get lfp value list.
	QString queryString = QString("SELECT dataid,fittedtime,channel,unit,waveform "
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
							,query.value(1).toDouble()
							,query.value(2).toInt()
							,query.value(3).toInt(),
							0,numEntries,wave)) );
		if(query.value(0).toLongLong() > lastSessionDataId_)
			lastSessionDataId_ = query.value(0).toLongLong();
		readQueries_++;
	}
	if(readQueries_ > totalQueries_)
		updateTotalQueryCount();
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