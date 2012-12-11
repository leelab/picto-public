#ifndef _SESSIONLOADER_H_
#define _SESSIONLOADER_H_
#include <QObject>
#include <QHash>
#include <QMutex>
#include <QTime>

#include <QSharedPointer>
#include "SessionState.h"
#include "SessionLoaderThread.h"

namespace Picto {
/*! \brief Component of Picto Playback system that loads data into SessionState.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionLoader : public QObject
#else
class SessionLoader : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionLoader(QSharedPointer<SessionState> sessState);
	virtual ~SessionLoader();

	QStringList getRunNames();
	void restart();
	bool loadRun(int runIndex);
	//Used to set the time up to which the system is currently attempting to process
	//data
	//Returns false if time is invalid (Not in current run)
	bool setCurrentTime(double time);
	//Sets the time up to which the system has finished processing data.
	void setProcessedTime(double time);
	bool runLoaded();
	double getMinBehavTime();
	double getMaxBehavTime();
	double getMinNeuralTime();
	double getMaxNeuralTime();
	double runDuration();
	bool dataIsReady(double time);

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
	virtual QVector<RunData> loadRunData() = 0;
	//Should be implemented to load all initial data necessary to bring experiment to a valid
	//initial state. Data should be loaded as if it all occured at time zero.
	virtual bool loadInitData(double upTo) = 0;
	//Should be implemented to load data starting at the time "beyond" and to the time "to"
	//including the timestamp of "to" but not including the timestamp of "beyond"
	//Return value is last value that was successfully read or negative if nothing was read.
	//subtractTime should be subtracted from all time values to bring "zero time" to the 
	//beginning of the current run.
	virtual double loadBehavData(double beyond,double to,double subtractTime) = 0;
	virtual double loadNeuralData(double beyond,double to,double subtractTime) = 0;

	void setBehavBounds(double min,double max);
	QSharedPointer<SessionState> sessionState_;

private slots:
	void loadData();

private:
	QVector<RunData> runs_;
	double minBehav_;
	double maxBehav_;
	double minNeural_;
	double maxNeural_;
	double runStart_;
	double runEnd_;
	double currTime_;
	double procTime_;
	double forwardBuffer_;
	double backBuffer_;
	bool runReset_;
	int runIndex_;
	QSharedPointer<QMutex> mutex_;
	QTime speedTimer_;
	QTime loadTimer_;
	QSharedPointer<SessionLoaderThread> loaderThread_;
};

}; //namespace Picto
#endif