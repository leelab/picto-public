#ifndef _FILESESSIONLOADER_H_
#define _FILESESSIONLOADER_H_

#include <QSqlDatabase>
#include <QVector>
#include "SessionLoader.h"

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
	QString getDesignDefinition();

protected:
	virtual QVector<RunData> loadRunData();
	virtual bool loadInitData(double upTo);
	virtual double loadBehavData(double after,double to,double subtractTime);
	virtual double loadNeuralData(double after,double to,double subtractTime);
private:
	bool getSignalInfo();
	bool loadDesignDefinition();
	QSqlDatabase session_;
	struct SigData
	{
		QString name_;
		QString tableName_;
		QStringList subChanNames_;
		double samplePeriod_;
	};
	QVector<SigData> sigs_;
	double dataBuffer_;
	QString designDef_;
};

}; //namespace Picto
#endif