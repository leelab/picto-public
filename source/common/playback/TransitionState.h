#ifndef _TRANSITIONSTATE_H_
#define _TRANSITIONSTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackTransData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class TransitionState : public TransitionReader, public ValueState
{
	Q_OBJECT
public:
	bool setTransition(double time,qulonglong dataId,int transId);

signals:
	void transitionActivated(int transId);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);
};

struct PlaybackTransData
{
	PlaybackTransData(){};
	PlaybackTransData(int transId){transId_ = transId;};
	int transId_;
};

}; //namespace Picto
#endif