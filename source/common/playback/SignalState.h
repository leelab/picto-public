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
class SignalState : public SignalReader, public ValueState
{
	Q_OBJECT
public:
	SignalState(QString name,QStringList subChanNames);
	bool setSignal(double time,qulonglong dataId,double sampPeriod,QByteArray dataArray);

signals:
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);
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