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

	bool setFile(QString path);
	QSharedPointer<DesignRoot> getDesignRoot();
	QStringList getRunNames();
	bool loadRun(int index);
	double runDuration(int index);
	double currRunDuration();
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

	virtual QVector<RunData> loadRunData();
	virtual bool loadInitData(double upTo);
	virtual double loadBehavData(double after,double to,double subtractTime);
	virtual double loadNeuralData(double after,double to,double subtractTime);
private:
	bool getSignalInfo();
	bool loadDesignDefinition();
	QSqlDatabase session_;
	QSharedPointer<SessionState> sessionState_;
	struct SigData
	{
		QString name_;
		QString tableName_;
		QStringList subChanNames_;
		double samplePeriod_;
	};
	QVector<SigData> sigs_;
	double dataBuffer_;
	QSharedPointer<DesignRoot> designRoot_;
	QHash<int,bool> obsoleteAssetIds_;
	int runIndex_;
};

}; //namespace Picto
#endif