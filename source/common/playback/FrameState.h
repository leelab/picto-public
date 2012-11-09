#ifndef _FRAMESTATE_H_
#define _FRAMESTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
 */
class FrameState : public FrameReader, public ValueState<qulonglong>
{
	Q_OBJECT
public:
	bool setFrame(qulonglong dataId,double frameTime);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(qulonglong index);
	virtual void requestMoreDataByTime(double time);
signals:
	void framePresented(double time);
};

}; //namespace Picto
#endif