#ifndef _PROPERTYSTATE_H_
#define _PROPERTYSTATE_H_
#include "ValueState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Property PlaybackData values for use in Playback system.
 */
class PropertyState : public PropertyReader, public ValueState<qulonglong>
{
	Q_OBJECT
public:
	PropertyState(int propId);
	bool setPropertyValue(double time,qulonglong dataId,QString value);
protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(qulonglong index);
	virtual void requestMoreDataByTime(double time);

signals:
	void propertyChanged(int propId, QString value);
private:
	int propId_;
};

}; //namespace Picto
#endif