#ifndef _TRANSITIONSTATE_H_
#define _TRANSITIONSTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackTransData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class TransitionState : public TransitionReader, public ValueState<qulonglong>
{
	Q_OBJECT
public:
	bool setTransition(double time,qulonglong dataId,int transId);
protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(qulonglong index);
	virtual void requestMoreDataByTime(double time);
signals:
	void transitionActivated(int transId);
};

struct PlaybackTransData
{
	PlaybackTransData(){};
	PlaybackTransData(int transId){transId_ = transId;};
	int transId_;
};

}; //namespace Picto
#endif