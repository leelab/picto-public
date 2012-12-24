#ifndef _SPIKESTATE_H_
#define _SPIKESTATE_H_
#include <QVector>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSpikeData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class SpikeState :public SpikeReader, public DataState
{
	Q_OBJECT
public:
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

signals:
	void spikeEvent(int channel, int unit, QVector<float> waveform);
protected:
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