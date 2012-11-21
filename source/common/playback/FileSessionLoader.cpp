#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "FileSessionLoader.h"
#include "../storage/SignalChannelInfo.h"
using namespace Picto;

FileSessionLoader::FileSessionLoader(QSharedPointer<SessionState> sessState) :
SessionLoader(sessState),
currRun_(0),
dataBuffer_(60.0)
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
	currRun_ = 0;

	//Intialize Object Data
	if(!buildRunsList())
		return false;
	if(!getSignalInfo())
		return false;
	if(!loadDesignDefinition())
		return false;
	return true;
}

void FileSessionLoader::setDataBufferTime(double timeSpan)
{
	if(timeSpan < 0)
		timeSpan = 0;
	dataBuffer_ = timeSpan;
}

QString FileSessionLoader::getDesignDefinition()
{
	return designDef_;
}

QStringList FileSessionLoader::getRunNames()
{
	QStringList returnVal;
	foreach(RunData run, runs_)
		returnVal.append(run.name_);
	return returnVal;
}

bool FileSessionLoader::loadRun(int runIndex)
{
	if(runIndex < 0 || runIndex >= runs_.size())
		return false;
	currRun_ = runIndex;
	return true;
}

void FileSessionLoader::childLoadData(PlaybackDataType type,PlaybackIndex currLast,PlaybackIndex to)
{
	if((!session_.isOpen()) || (currRun_ < 0))
		return;
	double startTime = currLast.time();
	double stopTime = to.time();
	if(startTime == stopTime)
		return;
	
	bool backward = stopTime < startTime;
	if(int((stopTime-startTime)/dataBuffer_) == 0)
		stopTime = backward?startTime-dataBuffer_:startTime+dataBuffer_;
	QSqlQuery query(session_);
	query.setForwardOnly(!backward);
	bool success;
	switch(type)
	{
	case eProperty:
		query.prepare("SELECT f.time,p.dataid,p.assetid,p.value FROM properties p, frames f "
			"WHERE f.dataid=p.frameid AND f.time > :starttime "
			"AND f.time <= :stoptime ORDER BY p.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		while(query.next()){
			sessionState_->setPropertyValue(query.value(0).toDouble(),query.value(1).toLongLong(),query.value(2).toInt(),query.value(3).toString());
		}
		query.finish();
		break;
	case eTransition:
		query.prepare("SELECT f.time,t.dataid,t.transid FROM transitions t, frames f "
			"WHERE f.dataid=t.frameid AND f.time > :starttime "
			"AND f.time <= :stoptime ORDER BY t.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		while(query.next()){
			sessionState_->setTransition(query.value(0).toDouble(),query.value(1).toLongLong(),query.value(2).toInt());
		}
		query.finish();
		break;
	case eFrame:
		query.prepare("SELECT f.dataid,f.time FROM frames f "
			"WHERE f.time > :starttime "
			"AND f.time <= :stoptime ORDER BY f.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		while(query.next()){
			sessionState_->setFrame(query.value(0).toLongLong(),query.value(1).toDouble());
		}
		query.finish();
		break;
	case eReward:
		query.prepare("SELECT r.dataid,r.duration,r.channel,r.time FROM rewards r "
			"WHERE r.time > :starttime "
			"AND r.time <= :stoptime ORDER BY r.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		while(query.next()){
			sessionState_->setReward(query.value(3).toDouble(),query.value(0).toLongLong(),query.value(1).toInt(),query.value(2).toInt());
		}
		query.finish();
		break;
	case eSignal:
		foreach(SigData sigData,sigs_)
		{
			query.prepare(QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"f.time > :starttime AND f.time <= :stoptime ORDER BY s.dataid").arg(sigData.tableName_));
			query.bindValue(":starttime",startTime);
			query.bindValue(":stoptime",stopTime);
			success = query.exec();
			if(!success)
			{
				Q_ASSERT(false);
				qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
				
				return;
			}
			while(query.next()){
				//Note: With signals, the definition is such that offsetTime after the frameTime of frameId is when the first signal data was read.
				sessionState_->setSignal(	sigData.name_,
											sigData.subChanNames_,
											query.value(1).toDouble()+query.value(2).toDouble(),
											query.value(0).toLongLong(),
											sigData.samplePeriod_,
											query.value(3).toByteArray()
											);
			}
			query.finish();
		}
		break;
	case eLfp:
		//setLFP(qulonglong dataId,double startTime,double sampPeriod,int channel,QByteArray data);
		break;
	case eSpike:
		//setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform);
		break;
	}
	
}

void FileSessionLoader::childLoadNextData(PlaybackDataType type,PlaybackIndex currLast,bool backward)
{
	if((!session_.isOpen()) || (currRun_ < 0))
		return;
	
	double startTime = currLast.time();
	double stopTime = backward?startTime-dataBuffer_:startTime+dataBuffer_;
	QSqlQuery query(session_);
	query.setForwardOnly(!backward);
	bool success;
	switch(type)
	{
	case eProperty:
		query.prepare("SELECT f.time,p.dataid,p.assetid,p.value FROM properties p, frames f "
			"WHERE f.dataid=p.frameid AND f.time > :starttime "
			"AND f.time <= :stoptime ORDER BY p.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		if(!query.next())
		{
			query.prepare("SELECT f.time,p.dataid,p.assetid,p.value FROM properties p, frames f "
				"WHERE f.dataid=p.frameid AND f.time > :starttime "
				"ORDER BY p.dataid LIMIT 1");
			query.bindValue(":starttime",startTime);
			success = query.exec();
			if(!success)
			{
				Q_ASSERT(false);
				qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
				
				return;
			}
			if(!query.next())
			{
				
				return;
			}
		}
		do{
			sessionState_->setPropertyValue(query.value(0).toDouble(),query.value(1).toLongLong(),query.value(2).toInt(),query.value(3).toString());
		}while(query.next());
		query.finish();
		break;
	case eTransition:
		query.prepare("SELECT f.time,t.dataid,t.transid FROM transitions t, frames f "
			"WHERE f.dataid=t.frameid AND f.time > :starttime "
			"AND f.time <= :stoptime ORDER BY t.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		if(!query.next())
		{
			query.prepare("SELECT f.time,t.dataid,t.transid FROM transitions t, frames f "
			"WHERE f.dataid=t.frameid AND f.time > :starttime "
			"ORDER BY t.dataid LIMIT 1");
			query.bindValue(":starttime",startTime);
			success = query.exec();
			if(!success)
			{
				Q_ASSERT(false);
				qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
				
				return;
			}
			if(!query.next())
			{
				
				return;
			}
		}
		do{
			sessionState_->setTransition(query.value(0).toDouble(),query.value(1).toLongLong(),query.value(2).toInt());
		}while(query.next());
		query.finish();
		break;
	case eFrame:
		query.prepare("SELECT f.dataid,f.time FROM frames f "
			"WHERE f.time > :starttime "
			"AND f.time <= :stoptime ORDER BY f.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		if(!query.next())
		{
			query.prepare("SELECT f.dataid,f.time FROM frames f "
			"WHERE f.time > :starttime "
			"ORDER BY f.dataid LIMIT 1");
			query.bindValue(":starttime",startTime);
			success = query.exec();
			if(!success)
			{
				Q_ASSERT(false);
				qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
				
				return;
			}
			if(!query.next())
			{
				
				return;
			}
		}
		do{
			sessionState_->setFrame(query.value(0).toLongLong(),query.value(1).toDouble());
		}while(query.next());
		query.finish();
		break;
	case eReward:
		query.prepare("SELECT r.dataid,r.duration,r.channel,r.time FROM rewards r "
			"WHERE r.time > :starttime "
			"AND r.time <= :stoptime ORDER BY r.dataid");
		query.bindValue(":starttime",startTime);
		query.bindValue(":stoptime",stopTime);
		success = query.exec();
		if(!success)
		{
			Q_ASSERT(false);
			qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
			
			return;
		}
		if(!query.next())
		{
			query.prepare("SELECT r.dataid,r.duration,r.channel,r.time FROM rewards r "
			"WHERE r.time > :starttime "
			"ORDER BY r.dataid LIMIT 1");
			query.bindValue(":starttime",startTime);
			success = query.exec();
			if(!success)
			{
				Q_ASSERT(false);
				qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
				
				return;
			}
			if(!query.next())
			{
				
				return;
			}
		}
		do{
			sessionState_->setReward(query.value(3).toDouble(),query.value(0).toLongLong(),query.value(1).toInt(),query.value(2).toInt());
		}while(query.next());
		query.finish();
		break;
	case eSignal:
		foreach(SigData sigData,sigs_)
		{
			query.prepare(QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"f.time > :starttime AND f.time <= :stoptime ORDER BY s.dataid").arg(sigData.tableName_));
			query.bindValue(":starttime",startTime);
			query.bindValue(":stoptime",stopTime);
			success = query.exec();
			if(!success)
			{
				Q_ASSERT(false);
				qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
				
				return;
			}

			if(!query.next())
			{
				query.prepare(QString("SELECT s.dataid,f.time,s.offsettime,s.data "
							"FROM %1 s,frames f WHERE f.dataid=s.frameid AND "
							"f.time > :starttime ORDER BY s.dataid LIMIT 1").arg(sigData.tableName_));
				query.bindValue(":starttime",startTime);
				success = query.exec();
				if(!success)
				{
					Q_ASSERT(false);
					qDebug("Failed to select data from table with error: " + query.lastError().text().toAscii());
					
					return;
				}
					if(!query.next())
						continue;
			}
			do{
				//Note: With signals, the definition is such that offsetTime after the frameTime of frameId is when the first signal data was read.
				sessionState_->setSignal(	sigData.name_,
											sigData.subChanNames_,
											query.value(1).toDouble()+query.value(2).toDouble(),
											query.value(0).toLongLong(),
											sigData.samplePeriod_,
											query.value(3).toByteArray()
											);
			}while(query.next());
			query.finish();
		}
		break;
	case eLfp:
		//setLFP(qulonglong dataId,double startTime,double sampPeriod,int channel,QByteArray data);
		break;
	case eSpike:
		//setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform);
		break;
	}
}

bool FileSessionLoader::buildRunsList()
{
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
		return false;
	}
	RunData run;
	while(query.next()){
		//continue if the run wasn't saved.
		if(!query.value(3).toBool())
			continue;
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
		runs_.append(run);
	}
	query.finish();
	return true;
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