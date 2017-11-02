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

/*! \brief Constructs a new FileSessionLoader.
 *	\details The FileSessionLoader will load file data into the input SessionState.
 */
FileSessionLoader::FileSessionLoader(QSharedPointer<SessionState> sessState)
: sessionState_(sessState),
runIndex_(-1),
sessionDataLoaded_(false)
{
	if(sessionState_)
		connect(sessionState_.data(),SIGNAL(percentLoaded(double)),this,SIGNAL(percentLoaded(double)));
}

/*! \brief Removes the connection to the current database before this FileSessionLoader is deleted.
*/
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

/*! \brief Sets the file path to the session file that will be loaded into the SessionState.
 */
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

/*! \brief Returns the DesignRoot for the Design loaded in loadDesignDefinition() when setFile() was called.
 *	\details If no design was loaded, an empty DesignRoot shared pointer will be returned.
 */
QSharedPointer<DesignRoot> FileSessionLoader::getDesignRoot()
{
	return designRoot_;
}

/*! \brief Returns a list of names of runs that were loaded from the session file.
*/
QStringList FileSessionLoader::getRunNames()
{
	QStringList returnVal;
	foreach(RunData runData,runs_)
	{
		returnVal.append(runData.name_);
	}
	return returnVal;
}

/*! \brief Returns a list of the notes saved by the operator for runs that were loaded from the session file.
 *	\details One entry appears for each name returned from getRunNames() and in the same order.
*/
QStringList FileSessionLoader::getRunNotes()
{
	QStringList returnVal;
	foreach(RunData runData,runs_)
	{
		returnVal.append(runData.notes_);
	}
	return returnVal;
}

/*! \brief Returns a list of the names of runs that were saved by the operator during the session.
*/
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

/*! \brief Loads the run at the input index (with respect to the list returned from getRunNames())
 *	into the SessionState for playback.
 *	\details Returns true if the run was successfully loaded, false otherwise.
 *	\note The first time this is called after a Session file is loaded, SessionState::setSessionData() is called
 *	which loads all session data (including every run) into the SessionState.  This will take a long time and lots of memory.
 *	On subsequent calls to this function, that is no longer necessary, so we just change the currently
 *	selected run.
 */
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

/*! \brief Returns the duration (in seconds) of the run at the input index, where the index is with
 *	respect to the list returned from getRunNames()
 */
double FileSessionLoader::runDuration(int index)
{
	if(index < 0 || index >= runs_.size())
		return 0;
	return runs_[index].endTime_-runs_[index].startTime_;
}

/*! \brief Returns the total duration of the currently loaded run.
 *	\sa loadRun()
 */
double FileSessionLoader::currRunDuration()
{
	return runDuration(runIndex_);
}

/*! \brief Returns the name of the currently loaded run.
 *	\sa loadRun()
 */
QString FileSessionLoader::currRunName()
{
	Q_ASSERT(runIndex_ >= 0 && runIndex_ < runs_.size());
	return runs_[runIndex_].name_;
}

/*! \brief Loads data about this sessions runs into this classes runs_ data structure for report in functions like
 *	getRunNames(), getRunNotes(), etc.
 */
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

/*! \brief Loads all information about which signals were used in the session and sets that data into the SessionState.
 *	\details This tells the SessionState how many objects it needs to handle loading of the Session's signal data
 *	and causes it to set them up properly.
 */
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
		//Add the signal to the session state so that it knows it should track its data
		if(sessionState_)
			sessionState_->addSignal(data.name_,data.tableName_,data.subChanNames_,data.samplePeriod_);
	}while(query.next());
	return true;
}

/*! \brief Loads the DesignRoot used in the session file so that it can be returned from getDesignRoot().
 *	\details This function needs to deal with interfacing different versions of Picto too.  When the DesignRoot
 *	is loaded, some of its Assets might be considered obsolete and upgraded automatically.  This means that some Asset ID values
 *	may change.  To be sure that the Asset ID values in the upgraded design match those in the session file, we use
 *	a SessionVersionInterfacer which fixes the upgraded DesignRoot's tree so that the AssetIDs match up with the values
 *	in the Session file.  The SessionVersionInterfacer also provides us with a list of "obsolete asset ids" which we can
 *	send into the SessionState so that changes to values of Properties that are children of Obsolete Assets can be 
 *	ignored.
 *	\sa SessionVersionInterfacer, SessionState::setSessionData()
 */
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

		//Plots
		int AssetID = query.value(0).toInt();
		if (!Assets_.keys().contains(AssetID))
		{
			QString name = query.value(1).toString();
			if (name != "")
				name = query.value(1).toString().split("::").last();
			if (name != "")
				Assets_.insert(query.value(0).toInt(), name);
		}
		//end Plots
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

/*! \brief Returns a QSqlDatabase object that can be used to query data from the Session file.
*/
QSqlDatabase FileSessionLoader::getDatabase()
{
	return QSqlDatabase::database(connectionName_);
}
QMap<int, QString> FileSessionLoader::getAssets()
{
	return Assets_;
}