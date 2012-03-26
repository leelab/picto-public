#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "SignalDataIterator.h"
#include "../storage/SignalChannelInfo.h"
using namespace Picto;

SignalDataIterator::SignalDataIterator(QSqlDatabase session,QString signalName)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	getSubChanInfo(signalName);
	totalQueries_ = 0;
	readValues_ = 0;
	readQueries_ = 0;
	numSubChans_ = 0;

	updateTotalQueryCount();
	getSubChanInfo(signalName);
}

SignalDataIterator::~SignalDataIterator()
{

}

QSharedPointer<SignalData> SignalDataIterator::getNextSignalVals()
{
	if(signalVals_.size())
		return signalVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updateSignalValsList();
	if(signalVals_.size())
		return signalVals_.takeFirst();
	//No new data, return an empty propData()
	return QSharedPointer<SignalData>(new SignalData());
}

void SignalDataIterator::updateSignalValsList()
{
	if(!isValid())
		return;
	if(readQueries_ >= totalQueries_)
		return;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Get signal value list.
	QString queryString = QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"s.dataid > :lastdataid ORDER BY s.dataid LIMIT 10000").arg(tableName_);
	query.prepare(queryString);
	query.bindValue(":lastdataid",lastSessionDataId_);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	qulonglong lastDataId = lastSessionDataId_;
	while(query.next()){
		//Convert data from blob to float array.  Get the relevant subchannel
		//and calculate its time.
		QByteArray dataByteArray = query.value(3).toByteArray();
		int numEntries = dataByteArray.size()/sizeof(float);
		float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
		for(int i=0;i<numEntries;i+=numSubChans_)
		{
			//Don't enter the data id.  Since the individual readings don't have data ids, but
			//do have different times, we want to use time as the index to be sure that it will
			//be possible to use eye readings as a trigger.
			//(ie. If we copied the dataid into the SignalData structs of each of a single row's
			//sub entries and we then used the signal as a trigger and a data source, we would 
			//end up missing signal points in the final readout.)
			signalVals_.append(QSharedPointer<SignalData>( new SignalData(
					0,
					query.value(1).toDouble()+query.value(2).toDouble()+((i/numSubChans_)*samplePeriod_),
					numSubChans_)));
			for(int j=0;j<numSubChans_;j++)
			{
				float newVal = floatArray[i+j];
				signalVals_.last()->values[j] = floatArray[i+j];
			}

			if(query.value(0).toLongLong() > lastDataId)
				lastDataId = query.value(0).toLongLong();
			readValues_++;
		}
		readQueries_++;
	}
	if(readQueries_ > totalQueries_)
		updateTotalQueryCount();
	
	lastSessionDataId_ = lastDataId;
}

void SignalDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT COUNT(dataid) FROM %1").arg(tableName_);
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

void SignalDataIterator::getSubChanInfo(QString signalName)
{
	numSubChans_ = -1;
	if(!session_.isOpen())
		return;
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	query.prepare("SELECT value FROM sessioninfo WHERE key='Signal'");
	bool success = query.exec();
	if(!success || !query.next())
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