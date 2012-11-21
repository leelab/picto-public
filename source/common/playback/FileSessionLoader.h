#ifndef _FILESESSIONLOADER_H_
#define _FILESESSIONLOADER_H_

#include <QSqlDatabase>
#include <QVector>
#include "SessionLoader.h"
#include "../../common/storage/TaskRunDataUnit.h"

namespace Picto {
/*! \brief Component of Picto Playback system that loads data into SessionState.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API FileSessionLoader : public SessionLoader
#else
class FileSessionLoader : public  QObject
#endif
{
	Q_OBJECT
public:
	FileSessionLoader(QSharedPointer<SessionState> sessState);
	virtual ~FileSessionLoader();

	bool setFile(QString path);
	void setDataBufferTime(double timeSpan);
	QString getDesignDefinition();

	virtual QStringList getRunNames();
	virtual bool loadRun(int runIndex);

protected:
	virtual void childLoadData(PlaybackDataType type,PlaybackIndex currLast,PlaybackIndex to);
	virtual void childLoadNextData(PlaybackDataType type,PlaybackIndex currLast,bool backward);
private:
	bool buildRunsList();
	bool getSignalInfo();
	bool loadDesignDefinition();
	QSqlDatabase session_;
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
	QVector<RunData> runs_;
	
	struct SigData
	{
		QString name_;
		QString tableName_;
		QStringList subChanNames_;
		double samplePeriod_;
	};
	QVector<SigData> sigs_;
	int currRun_;
	double dataBuffer_;
	QString designDef_;
};

}; //namespace Picto
#endif