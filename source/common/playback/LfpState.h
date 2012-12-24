#ifndef _LFPSTATE_H_
#define _LFPSTATE_H_
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class LfpState : public LfpReader, public DataState
{
	Q_OBJECT
public:
	LfpState(int channel,double sampPeriod);
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

signals:
	void lfpChanged(int channel,double value);
//
//protected:
//	virtual void triggerValueChange(bool reverse,bool last);
//	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
//	virtual void requestNextData(PlaybackIndex currLast,bool backward);
private:
	int channel_;
	int sampPeriod_;
};

}; //namespace Picto
#endif