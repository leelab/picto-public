#ifndef _LFPSTATE_H_
#define _LFPSTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class LfpState : public LfpReader, public ValueState
{
	Q_OBJECT
public:
	LfpState(int channel,double sampPeriod);
	bool setLFP(qulonglong dataid,double startTime,int channel,QByteArray dataArray);
signals:
	void lfpChanged(int channel,double value);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);
private:
	int channel_;
	int sampPeriod_;
};

}; //namespace Picto
#endif