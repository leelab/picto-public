#ifndef _SIGNALSTATE_H_
#define _SIGNALSTATE_H_
#include <QVector>
#include <QStringList>
#include <QHash>
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

	//Signal Reader Interface
	//Returns the name of the signal handled by this signal reader
	virtual QString getName();
	//Returns the sub channels of this signal
	virtual QStringList getComponentNames();
	virtual double getSamplePeriod();
	virtual double getLatestTime();
	virtual double getLatestValue(QString channel);
	virtual double getNextTime();
	virtual double getNextValue(QString channel);
	//Returns signal values for the input sub channel with times > the input time.
	virtual QVariantList getValuesSince(QString channel,double time);
	virtual QVariantList getValuesUntil(QString channel,double time);

signals:
	void signalChanged(QString name, QStringList subChanNames,QVector<float> vals);

private:
	void goToNext();
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	bool moveIndecesToNextTime(int& outerIndex, int& innerIndex);
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
	QHash<QString,int> subChanIndexLookup_;
	int numSubChans_;
	double sampPeriod_;
};

struct PlaybackSignalData
{
	PlaybackSignalData(){};
	PlaybackSignalData(double time){time_= time;};
	inline bool operator<(const PlaybackSignalData& someData) const {
		return time_ < someData.time_;
	}
	double time_;
	QVector<float> vals_;
};

}; //namespace Picto
#endif