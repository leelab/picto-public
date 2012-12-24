#ifndef _SIGNALSTATE_H_
#define _SIGNALSTATE_H_
#include <QVector>
#include <QStringList>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSignalData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class SignalState : public SignalReader, public DataState
{
	Q_OBJECT
public:
	SignalState(QString name,QString tableName,QStringList subChanNames,double sampPeriod);

	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

signals:
	void signalChanged(QString name, QStringList subChanNames,QVector<float> vals);
protected:
	//bool querySessionForData(QSharedPointer<QSqlQuery> query,double after,double upTo);
	//QVector<QSharedPointer<IndexedData>> convertQueryToDataVector(QSharedPointer<QSqlQuery> query, double zeroTime);
	//void triggerDataChange(QSharedPointer<IndexedData> data);

private:
	void goToNext();
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	int currSub_;
	QVector<PlaybackSignalData> data_;

	QString name_;
	QString tableName_;
	QStringList subChanNames_;
	int numSubChans_;
	double sampPeriod_;
//	bool setSignal(double time,qulonglong dataId,double sampPeriod,QByteArray dataArray);
//
//signals:
//	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
//	void needsData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsNextData(PlaybackIndex currLast,bool backward);
//
//protected:
//	virtual void triggerValueChange(bool reverse,bool last);
//	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
//	virtual void requestNextData(PlaybackIndex currLast,bool backward);
};

struct PlaybackSignalData
{
	PlaybackSignalData(){};
	PlaybackSignalData(double time){time_= time;};
	double time_;
	QVector<float> vals_;
};

}; //namespace Picto
#endif