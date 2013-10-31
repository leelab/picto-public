#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "FileSessionLoader.h"
#include "../storage/SignalChannelInfo.h"
#include "../experiment/Experiment.h"
#include "SessionVersionInterfacer.h"
using namespace Picto;

QHash<QString,int> FileSessionLoader::connectionUsers_;

FileSessionLoader::FileSessionLoader(QSharedPointer<SessionState> sessState)
: sessionState_(sessState),
runIndex_(-1),
sessionDataLoaded_(false)
{
	if(sessionState_)
		connect(sessionState_.data(),SIGNAL(percentLoaded(double)),this,SIGNAL(percentLoaded(double)));
}

FileSessionLoader::~FileSessionLoader()
{
	sessionState_.clear();
	if(connectionName_.isEmpty())
		return;
	Q_ASSERT(connectionUsers_.contains(connectionName_));
	connectionUsers_[connectionName_]--;
	if(connectionUsers_[connectionName_] <= 0)
	{
		QSqlDatabase::database(connectionName_).close();
		QSqlDatabase::removeDatabase(connectionName_);
		connectionUsers_.remove(connectionName_);
	}
}

bool FileSessionLoader::setFile(QString path)
{
	sessionDataLoaded_ = false;
	Q_ASSERT(connectionName_.isEmpty());	//Currently, we just create a new file session loader whenever we need to load a new file
											//
	//Load Sqlite Database
	path = path.replace("\\","/");
	Q_ASSERT(path.lastIndexOf("/") >=0);
	Q_ASSERT(path.lastIndexOf(".") >=0);
	QString connectionName = path.mid(path.lastIndexOf("/")+1);
	connectionName = connectionName.left(connectionName.lastIndexOf("."));
	Q_ASSERT(QSqlDatabase::contains(connectionName) == connectionUsers_.contains(connectionName));
	if(!QSqlDatabase::contains(connectionName))
	{
		QSqlDatabase newSession = QSqlDatabase::addDatabase("QSQLITE",connectionName);
		newSession.setDatabaseName(path);
		newSession.open();
		if(!newSession.isOpen())
		{
			newSession.close();
			QSqlDatabase::removeDatabase(connectionName);
			qDebug("Error: Could not open session file.");
			return false;
		}
		connectionUsers_[connectionName] = 0;
	}
	connectionUsers_[connectionName]++;
	connectionName_ = connectionName;

	//Intialize Object Data
	if(!loadRunData())
		return false;
	if(!getSignalInfo())
		return false;
	if(!loadDesignDefinition())
		return false;
	runIndex_ = -1;
	return true;
}

QSharedPointer<DesignRoot> FileSessionLoader::getDesignRoot()
{
	return designRoot_;
}

QStringList FileSessionLoader::getRunNames()
{
	QStringList returnVal;
	foreach(RunData runData,runs_)
	{
		returnVal.append(runData.name_);
	}
	return returnVal;
}

QStringList FileSessionLoader::getRunNotes()
{
	QStringList returnVal;
	foreach(RunData runData,runs_)
	{
		returnVal.append(runData.notes_);
	}
	return returnVal;
}

QStringList FileSessionLoader::getSavedRunNames()
{
	QStringList returnVal;
	foreach(RunData runData,runs_)
	{
		if(runData.saved_)
			returnVal.append(runData.name_);
	}
	return returnVal;
}

bool FileSessionLoader::loadRun(int index)
{
	if(runIndex_ == index)
		return true;
	if(index < 0 || index >= runs_.size())
		return false;
	if(!sessionState_)
		return false;
	runIndex_ = index;
	if(!sessionDataLoaded_)
	{
		try
		{	//Adding the data base to the session state involves creating
			//lots of giant data structures in RAM to hold everything.  If
			//there is any RAM limitation, or even too much RAM fragmentation
			//this might fail, so check for that here.
			sessionState_->setSessionData(getDatabase(),obsoleteAssetIds_);
		}
		catch(...)
		{	//Looks like loading failed... return false;
			return false;
		}
		sessionDataLoaded_ = true;
	}
	sessionState_->startNewRun(runs_[index].startTime_,runs_[index].endTime_);
	return true;
}

double FileSessionLoader::runDuration(int index)
{
	if(index < 0 || index >= runs_.size())
		return 0;
	return runs_[index].endTime_-runs_[index].startTime_;
}

double FileSessionLoader::currRunDuration()
{
	return runDuration(runIndex_);
}

QString FileSessionLoader::currRunName()
{
	Q_ASSERT(runIndex_ >= 0 && runIndex_ < runs_.size());
	return runs_[runIndex_].name_;
}

bool FileSessionLoader::loadRunData()
{
	runs_.clear();
	QSqlQuery query(getDatabase());
	query.setForwardOnly(true);

	//Get runs list.
	query.prepare("SELECT r.runid,r.name,r.notes,r.saved,r.firstframe,r.lastframe,s.time,e.time "
		"FROM runs r, frames s, frames e WHERE s.dataid=r.firstframe AND ((r.lastframe>0 AND e.dataid=r.lastframe) "
		"OR (r.lastframe=0 AND e.dataid=(SELECT dataid FROM frames ORDER BY dataid DESC LIMIT 1))) ORDER BY runid");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Failed to select data from runs table with error: " + query.lastError().text().toLatin1());
		return false;
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
		runs_.append(run);
	}
	query.finish();
	if(runs_.size() == 0)
		return false;
	return true;
}

double FileSessionLoader::loadNeuralData(double after,double to,double subtractTime)
{
	if(!getDatabase().isOpen())
		return after;
	if(after == to)
		return after;
	
	QSqlQuery query(getDatabase());
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
	QSqlQuery query(getDatabase());
	query.setForwardOnly(true);
	query.prepare("SELECT value FROM sessioninfo WHERE key='Signal'");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
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
		if(sessionState_)
			sessionState_->addSignal(data.name_,data.tableName_,data.subChanNames_,data.samplePeriod_);
	}while(query.next());
	return true;
}

bool FileSessionLoader::loadDesignDefinition()
{
	////////////////////////
	//Load design definition
	/////////////////////////
	QSqlQuery query(getDatabase());
	query.setForwardOnly(true);
	//ExperimentXML name changed to DesignXML when we started putting UI Data and Analysis Data together
	//with it on server.  They never appeared at the same time so we can just look for either one.
	query.prepare("SELECT value FROM sessioninfo WHERE key='ExperimentXML' OR key='DesignXML'");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
		return false;
	}
	if(!query.next())
	{
		return false;
	}
	QString designDef = query.value(0).toString();
	
	/////////////////////////
	//Create the design root
	/////////////////////////
	designRoot_ = QSharedPointer<DesignRoot>(new DesignRoot());
	if(!designRoot_->resetDesignRoot(designDef) || !designRoot_->compiles())
	{
		Q_ASSERT(false);	//This would mean that somehow the session had a bad design in it.
		return false;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	//Since asset ids can change for unsaved (default) asset values when the same experiment 
	//is loaded in different versions of picto, us a SessionVersionInterfacer to correct the
	//asset ids in the new version of the experiment.
	/////////////////////////////////////////////////////////////////////////////////////////
	QSharedPointer<Picto::Experiment> experiment = designRoot_->getExperiment().staticCast<Experiment>();
	if(!experiment)
	{
		Q_ASSERT(false);	//This would mean that somehow the session had no experiment in it.
		return false;
	}
	SessionVersionInterfacer updater(experiment);
	////////////////////////////////////////////////////////
	//Add all Session asset data to SessionVersionInterfacer
	////////////////////////////////////////////////////////
	query.prepare("SELECT assetid,name,parent FROM propertylookup WHERE parent <> 0");
	success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
		return false;
	}
	while(query.next())
	{
		updater.addSessionProperty(query.value(1).toString(),query.value(0).toInt(),query.value(2).toInt());
	}

	query.prepare("SELECT source,sourceresult,assetid,parent FROM transitionlookup");
	success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
		return false;
	}
	while(query.next())
	{
		updater.addSessionTransition(query.value(0).toString(),query.value(1).toString(),query.value(2).toInt(),query.value(3).toInt());
	}

	query.prepare("SELECT assetid,path FROM elementlookup");
	success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toLatin1());
		return false;
	}
	while(query.next())
	{
		updater.addSessionElement(query.value(0).toInt(),query.value(1).toString());
	}

	////////////////////////////////////////////////////////
	//Update Experiment's DesignConfig
	////////////////////////////////////////////////////////
	updater.updateSessionConfigFromSessionAssets();

	////////////////////////////////////////////////////////
	//Get obsolete Asset Id table
	////////////////////////////////////////////////////////
	obsoleteAssetIds_ = updater.getObsoleteAssets();
	return true;
}

QSqlDatabase FileSessionLoader::getDatabase()
{
	return QSqlDatabase::database(connectionName_);
}