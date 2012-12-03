#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "FileSessionLoader.h"
#include "../storage/SignalChannelInfo.h"
using namespace Picto;

FileSessionLoader::FileSessionLoader(QSharedPointer<SessionState> sessState) :
SessionLoader(sessState)
{
}

FileSessionLoader::~FileSessionLoader()
{
}

bool FileSessionLoader::setFile(QString path)
{
	//Load Sqlite Database
	path = path.replace("\\","/");
	Q_ASSERT(path.lastIndexOf("/") >=0);
	Q_ASSERT(path.lastIndexOf(".") >=0);
	QString connectionName = path.mid(path.lastIndexOf("/")+1);
	connectionName = connectionName.left(connectionName.lastIndexOf("."));

	QSqlDatabase newSession = QSqlDatabase::addDatabase("QSQLITE",connectionName);
	newSession.setDatabaseName(path);
	newSession.open();
	if(!newSession.isOpen())
	{
		qDebug("Error: Could not open session file.");
		return false;
	}
	session_ = newSession;

	//Intialize Object Data
	if(!getSignalInfo())
		return false;
	if(!loadDesignDefinition())
		return false;
	return true;
}

QString FileSessionLoader::getDesignDefinition()
{
	return designDef_;
}

QVector<SessionLoader::RunData> FileSessionLoader::loadRunData()
{
	QVector<RunData> runs;
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Get runs list.
	query.prepare("SELECT r.runid,r.name,r.notes,r.saved,r.firstframe,r.lastframe,s.time,e.time "
		"FROM runs r, frames s, frames e WHERE s.dataid=r.firstframe AND ((r.lastframe>0 AND e.dataid=r.lastframe) "
		"OR (r.lastframe=0 AND e.dataid=(SELECT dataid FROM frames ORDER BY dataid DESC LIMIT 1))) ORDER BY runid");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Failed to select data from runs table with error: " + query.lastError().text().toAscii());
		return runs;
	}

	RunData run;
	while(query.next()){
		////continue if the run wasn't saved.
		//if(!query.value(3).toBool())
		//	continue;
		//Build up the run object
		run.dataId_ = query.value(0).toLongLong();
		run.startFrame_ = query.value(4).toLongLong();
		run.endFrame_ = query.value(5).toLongLong();
		run.name_ = query.value(1).toString();
		run.notes_ = query.value(2).toString();
		run.saved_ = query.value(3).toBool();
		run.startTime_ = query.value(6).toDouble();
		run.endTime_ = query.value(7).toDouble();
		//Add RunData to the runs_ list.
		runs.append(run);
	}
	query.finish();
	return runs;
}

bool FileSessionLoader::loadInitData(double upTo)
{
	if(!session_.isOpen())
		return false;
	if(upTo <= 0)
		return true;
	
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	bool success;
	//Property:
	query.prepare("SELECT p.dataid,p.assetid,p.value FROM properties p, frames f "
		"WHERE f.dataid=p.frameid "
		"AND f.time < :upto ORDER BY p.dataid");
	query.bindValue(":upto",upTo);
	success = query.exec();
	if(!success)
	{
		Q_ASSERT(false);
		qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
		return false;
	}
	while(query.next()){
		sessionState_->setPropertyValue(0,query.value(0).toLongLong(),query.value(1).toInt(),query.value(2).toString());
	}
	query.finish();
	return true;
}

double FileSessionLoader::loadBehavData(double after,double to,double subtractTime)
{
	double returnVal = after;
	if(!session_.isOpen())
		return returnVal;
	if(after == to)
		return returnVal;
	
	double time = -1;
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	bool success;
	//Property:
	query.prepare("SELECT f.time,p.dataid,p.assetid,p.value FROM properties p, frames f "
		"WHERE f.dataid=p.frameid AND f.time > :after "
		"AND f.time <= :to ORDER BY p.dataid");
	query.bindValue(":after",after);
	query.bindValue(":to",to);
	success = query.exec();
	if(!success)
	{
		Q_ASSERT(false);
		qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
		return after;
	}
	while(query.next()){
		time = query.value(0).toDouble();
		sessionState_->setPropertyValue(time-subtractTime,query.value(1).toLongLong(),query.value(2).toInt(),query.value(3).toString());
		if(time > returnVal)
			returnVal = time;
	}
	query.finish();
	//eTransition:
	query.prepare("SELECT f.time,t.dataid,t.transid FROM transitions t, frames f "
		"WHERE f.dataid=t.frameid AND f.time > :after "
		"AND f.time <= :to ORDER BY t.dataid");
	query.bindValue(":after",after);
	query.bindValue(":to",to);
	success = query.exec();
	if(!success)
	{
		Q_ASSERT(false);
		qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
		return after;
	}
	while(query.next()){
		time = query.value(0).toDouble();
		sessionState_->setTransition(time-subtractTime,query.value(1).toLongLong(),query.value(2).toInt());
		if(time > returnVal)
			returnVal = time;
	}
	query.finish();
	//eFrame:
	query.prepare("SELECT f.dataid,f.time FROM frames f "
		"WHERE f.time > :after "
		"AND f.time <= :to ORDER BY f.dataid");
	query.bindValue(":after",after);
	query.bindValue(":to",to);
	success = query.exec();
	if(!success)
	{
		Q_ASSERT(false);
		qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
		return after;
	}
	while(query.next()){
		time = query.value(1).toDouble();
		sessionState_->setFrame(query.value(0).toLongLong(),time-subtractTime);
		if(time > returnVal)
			returnVal = time;
	}
	query.finish();
	//eReward:
	query.prepare("SELECT r.dataid,r.duration,r.channel,r.time FROM rewards r "
		"WHERE r.time > :after "
		"AND r.time <= :to ORDER BY r.dataid");
	query.bindValue(":after",after);
	query.bindValue(":to",to);
	success = query.exec();
	if(!success)
	{
		Q_ASSERT(false);
		qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
		return after;
	}
	while(query.next()){
		time = query.value(3).toDouble();
		sessionState_->setReward(time-subtractTime,query.value(0).toLongLong(),query.value(1).toInt(),query.value(2).toInt());
		if(time > returnVal)
			returnVal = time;
	}
	query.finish();
	//eSignal:
	foreach(SigData sigData,sigs_)
	{
		query.prepare(QString("SELECT s.dataid,f.time,s.offsettime,s.data "
						"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
						"f.time > :after AND f.time <= :to ORDER BY s.dataid").arg(sigData.tableName_));
		query.bindValue(":after",after);
		query.bindValue(":to",to);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return after;
		}
		while(query.next()){
			time = query.value(1).toDouble();
			//Note: With signals, the definition is such that offsetTime after the frameTime of frameId is when the first signal data was read.
			sessionState_->setSignal(	sigData.name_,
										sigData.subChanNames_,
										time+query.value(2).toDouble()-subtractTime,
										query.value(0).toLongLong(),
										sigData.samplePeriod_,
										query.value(3).toByteArray()
										);
			if(time > returnVal)
				returnVal = time;
		}
		query.finish();
	}
	return returnVal;
}

double FileSessionLoader::loadNeuralData(double after,double to,double subtractTime)
{
	if(!session_.isOpen())
		return after;
	if(after == to)
		return after;
	
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	bool success;
	// eLfp:
	//setLFP(qulonglong dataId,double startTime,double sampPeriod,int channel,QByteArray data);
	//eSpike:
	//setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform);
	return to;
}

bool FileSessionLoader::getSignalInfo()
{
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	query.prepare("SELECT value FROM sessioninfo WHERE key='Signal'");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return false;
	}
	if(!query.next())
	{
		return false;
	}
	do
	{
		SignalChannelInfo sigInf;
		sigInf.fromXml(query.value(0).toString());
		SigData data;
		data.name_ = sigInf.getName();
		data.tableName_ = sigInf.getTableName();
		data.subChanNames_ = sigInf.getSubchannelNames().split(",",QString::SkipEmptyParts);
		data.samplePeriod_ = double(sigInf.getResolution())/1000.0;
		sigs_.append(data);
		//Add the signal to the session state so that it knows it should track its data
		sessionState_->addSignal(data.name_,data.subChanNames_);
	}while(query.next());
	return true;
}

bool FileSessionLoader::loadDesignDefinition()
{
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	query.prepare("SELECT value FROM sessioninfo WHERE key='ExperimentXML'");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return false;
	}
	if(!query.next())
	{
		return false;
	}
	designDef_ = query.value(0).toString();
	return true;
}