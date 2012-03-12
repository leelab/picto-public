#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "SignalDataIterator.h"
#include "../storage/SignalChannelInfo.h"
using namespace Picto;

SignalDataIterator::SignalDataIterator(QSqlDatabase session,QString signalName,QString subChannelName)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	getSubChanInfo(signalName,subChannelName);
	totalValues_ = 0;
}

SignalDataIterator::~SignalDataIterator()
{

}

SignalData SignalDataIterator::getNextSignalVals()
{
	if(signalVals_.size())
		return signalVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updateSignalValsList();
	if(signalVals_.size())
		return signalVals_.takeFirst();
	//No new data, return an empty propData()
	return SignalData();
}

void SignalDataIterator::updateSignalValsList()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	
	//Get dataid of last frame in session
	query.prepare("SELECT dataid FROM frames ORDER BY time DESC LIMIT 1");
	bool success = query.exec();
	if(!success || !query.next())
	{
		lastSessionDataId_ = 0;
		return;
	}
	qulonglong lastFrameDataId = query.value(0).toLongLong();

	//Get signal value list.
	QString queryString = QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"s.dataid > :lastdataid ORDER BY s.dataid").arg(tableName_);
	query.prepare(queryString);
	query.bindValue(":lastdataid",lastSessionDataId_);
	success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	qulonglong lastDataId = 0;
	while(query.next()){
		//Convert data from blob to float array.  Get the relevant subchannel
		//and calculate its time.
		QByteArray dataByteArray = query.value(3).toByteArray();
		int numEntries = dataByteArray.size()/sizeof(float);
		float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
		for(int i=subChanIndex_;i<numEntries;i+=numSubChans_)
		{
			//Don't enter the data id.  Since the individual readings don't have data ids, but
			//do have different times, we want to use time as the index to be sure that it will
			//be possible to use eye readings as a trigger.
			//(ie. If we copied the dataid into the SignalData structs of each of a single row's
			//sub entries and we then used the signal as a trigger and a data source, we would 
			//end up missing signal points in the final readout.)
			signalVals_.append(SignalData(
					0,
					query.value(1).toDouble()+query.value(2).toDouble()+((i/numSubChans_)*samplePeriod_),
					floatArray[i]));
			if(query.value(0).toLongLong() > lastDataId)
				lastDataId = query.value(0).toLongLong();
		}
	}
	totalValues_ = signalVals_.size();

	//Update lastSessionDataId_ to either the last frame in the session or the last signal dataid.  
	//Whichever is higher.
	if(lastDataId > lastFrameDataId)
		lastSessionDataId_ = lastDataId;
	else
		lastSessionDataId_ = lastFrameDataId;
}

void SignalDataIterator::getSubChanInfo(QString signalName, QString subChanName)
{
	subChanIndex_ = -1;
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
		samplePeriod_ = double(sigInf.getResolution())/1000.0;
		QString subChanNames = sigInf.getSubchannelNames();
		QStringList subChanNameList = subChanNames.split(",",QString::SkipEmptyParts);
		int i=0;
		foreach(QString name,subChanNameList)
		{
			if(name.toLower() == subChanName.toLower())
			{
				subChanIndex_ = i;
				return;
			}
			i++;
		}
	}while(query.next());
}