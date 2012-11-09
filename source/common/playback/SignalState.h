#ifndef _SIGNALSTATE_H_
#define _SIGNALSTATE_H_
#include <QVector>
#include <QStringList>
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSignalData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class SignalState : public SignalReader, public ValueState<double>
{
	Q_OBJECT
public:
	SignalState(QString name,QStringList subChanNames);
	bool setSignal(double time,qulonglong dataId,double sampPeriod,QByteArray dataArray);
protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(double index);
	virtual void requestMoreDataByTime(double time);
signals:
	void signalChanged(QString name,QVector<float> vals);
private:
	QString name_;
	QStringList subChanNames_;
	int numSubChans_;
};

struct PlaybackSignalData
{
	PlaybackSignalData(){};
	PlaybackSignalData(QVector<float> vals){vals_ = vals;};
	QVector<float> vals_;
};

}; //namespace Picto
#endif