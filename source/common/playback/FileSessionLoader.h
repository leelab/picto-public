#ifndef _FILESESSIONLOADER_H_
#define _FILESESSIONLOADER_H_

#include <QSqlDatabase>
#include <QVector>
#include <QObject>
#include "SessionState.h"
//#include "SessionLoader.h"
#include "../design/DesignRoot.h"

namespace Picto {
/*! \brief Component of Picto Playback system that loads data into SessionState.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API FileSessionLoader : public QObject
#else
class FileSessionLoader : public  QObject
#endif
{
	Q_OBJECT
public:
	FileSessionLoader(QSharedPointer<SessionState> sessState);
	~FileSessionLoader();

	bool setFile(QString path);
	QSharedPointer<DesignRoot> getDesignRoot();
	QStringList getRunNames();
	QStringList getSavedRunNames();
	bool loadRun(int index);
	double runDuration(int index);
	double currRunDuration();
	QString currRunName();
signals:
	void percentLoaded(double percent);

protected:
	struct RunData
	{
		qulonglong dataId_;
		qulonglong startFrame_;
		qulonglong endFrame_;
		QString name_;
		QString notes_;
		bool saved_;
		double startTime_;
		double endTime_;
	};

	virtual bool loadRunData();
	virtual bool loadInitData(double upTo);
	virtual double loadBehavData(double after,double to,double subtractTime);
	virtual double loadNeuralData(double after,double to,double subtractTime);
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
	QVector<SigData> sigs_;
	QVector<RunData> runs_;
	double dataBuffer_;
	QSharedPointer<DesignRoot> designRoot_;
	QHash<int,bool> obsoleteAssetIds_;
	int runIndex_;
	bool sessionDataLoaded_;
	static QHash<QString,int> connectionUsers_;
};

}; //namespace Picto
#endif