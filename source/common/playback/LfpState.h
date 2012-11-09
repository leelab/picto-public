#ifndef _LFPSTATE_H_
#define _LFPSTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class LfpState : public LfpReader, public ValueState<double>
{
	Q_OBJECT
public:
	bool setLFP(qulonglong dataid,double startTime,int channel,QByteArray data);
protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(double index);
	virtual void requestMoreDataByTime(double time);
signals:
	void lfpChanged(int channel,double value);
};

}; //namespace Picto
#endif