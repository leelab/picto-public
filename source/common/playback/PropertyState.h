#ifndef _PROPERTYSTATE_H_
#define _PROPERTYSTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Property PlaybackData values for use in Playback system.
 */
class PropertyState : public PropertyReader, public ValueState
{
	Q_OBJECT
public:
	PropertyState(int propId);
	bool setPropertyValue(double time,qulonglong dataId,QString value);

signals:
	void propertyChanged(int propId, QString value);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);

private:
	int propId_;
};

}; //namespace Picto
#endif