#ifndef _SPIKESTATE_H_
#define _SPIKESTATE_H_
#include <QVector>
#include "EventState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSpikeData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class SpikeState :public SpikeReader, public EventState
{
	Q_OBJECT
public:
	bool setSpike(qulonglong dataid,double spikeTime,int channel,int unit,QByteArray waveform);
signals:
	void spikeEvent(int channel, int unit, QVector<float> waveform);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);
};

struct PlaybackSpikeData
{
	PlaybackSpikeData(){};
	PlaybackSpikeData(int channel, int unit, QVector<float> waveform){channel_=channel;unit_=unit;waveform_ = waveform;};
	int channel_;
	int unit_;
	QVector<float> waveform_;
};

}; //namespace Picto
#endif