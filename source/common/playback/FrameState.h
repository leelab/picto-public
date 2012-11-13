#ifndef _FRAMESTATE_H_
#define _FRAMESTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
 */
class FrameState : public FrameReader, public ValueState
{
	Q_OBJECT
public:
	bool setFrame(qulonglong dataId,double frameTime);

signals:
	void framePresented(double time);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);
};

}; //namespace Picto
#endif