#ifndef _FILESESSIONLOADER_H_
#define _FILESESSIONLOADER_H_

#include <QSqlDatabase>
#include <QVector>
#include <QObject>
#include "SessionState.h"
//#include "SessionLoader.h"
#include "../design/DesignRoot.h"

namespace Picto {
/*! \brief A Component of the Picto Playback system that loads data from a Session file into a SessionState.
 *	\details This class takes care of loading the session file, extracting various meta-data like task run
 *	information and the types of input signals used in the experiment, and setting all of this data into a 
 *	SessionState.
 *
 *	\note The most confusing part of what a FileSessionLoader does is the upgrade of the Design used in the session
 *	file.  Since the Session might have been recorded when an older version of Picto was in use, when the Design
 *	is loaded into this version of Picto, some Assets might be upgraded automatically.  Whenever this happens though,
 *	there is a possibility that Assets that were not upgraded might have their Asset ids changed for various reasons.
 *	This class uses a SessionVersionInterfacer to automatically fix the changed Asset IDs and extract the Asset IDs of
 *	Assets that are now obsolete so that their Property values can be ignored.
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API FileSessionLoader : public QObject
#else
class FileSessionLoader : public  QObject
#endif
{
	Q_OBJECT
public:
	FileSessionLoader(QSharedPointer<SessionState> sessState = QSharedPointer<SessionState>());
	~FileSessionLoader();

	bool setFile(QString path);
	QSharedPointer<DesignRoot> getDesignRoot();
	QStringList getRunNames();
	QStringList getRunNotes();
	QStringList getSavedRunNames();
	bool loadRun(int index);
	double runDuration(int index);
	double currRunDuration();
	QString currRunName();

	QMap<int, QString> getAssets();
signals:
	void percentLoaded(double percent);

protected:
	/*! \brief Stores data about a particular Task Run.*/
	struct RunData
	{
		qulonglong dataId_;			//!< The Task Run's DataID
		qulonglong startFrame_;		//!< The frame ID of the first frame in the run.
		qulonglong endFrame_;		//!< The frame ID of the last frame in the run.
		QString name_;				//!< The name of the run as set by the operator in the RemoteViewer
		QString notes_;				//!< The notes for the run as set by the operator in the RemoteViewer
		bool saved_;				//!< A boolean indicating if the Operator marked this run as Saved.
		double startTime_;			//!< The time at which the run started (this is the time of the startFrame_)
		double endTime_;			//!< The time at which the run ended (this is the time of the endFrame_)
	};

	virtual bool loadRunData();
private:
	bool getSignalInfo();
	bool loadDesignDefinition();
	QSqlDatabase getDatabase(); 
	QString connectionName_;
	QSharedPointer<SessionState> sessionState_;
	struct SigData
	{
		QString name_;
		QString tableName_;
		QStringList subChanNames_;
		double samplePeriod_;
	};
	QVector<RunData> runs_;
	double dataBuffer_;
	QSharedPointer<DesignRoot> designRoot_;
	QHash<int,bool> obsoleteAssetIds_;
	int runIndex_;
	bool sessionDataLoaded_;
	static QHash<QString,int> connectionUsers_;

	QMap<int, QString> Assets_;
};

}; //namespace Picto
#endif