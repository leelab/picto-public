#ifndef _TRANSITIONSTATE_H_
#define _TRANSITIONSTATE_H_
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackTransData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class TransitionState : public TransitionReader, public DataState
{
	Q_OBJECT
public:
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);
	void setObsoleteAssets(QHash<int,bool> obsoleteAssetIds){obsoleteAssetIds_ = obsoleteAssetIds;};
	int getFirstTransIdInRun();
signals:
	void transitionActivated(int transId);
private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	int firstLocationInRun_;
	QList<PlaybackTransData> data_;
	QHash<int,bool> obsoleteAssetIds_;
//public:
//	bool setTransition(double time,qulonglong dataId,int transId);
//
//signals:
//	void transitionActivated(int transId);
//	void needsData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsNextData(PlaybackIndex currLast,bool backward);
//
//protected:
//	virtual void triggerValueChange(bool reverse,bool last);
//	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
//	virtual void requestNextData(PlaybackIndex currLast,bool backward);
};

struct PlaybackTransData
{
	PlaybackTransData(){};
	PlaybackTransData(PlaybackIndex index,int transId){index_ = index;transId_ = transId;};
	PlaybackIndex index_;
	int transId_;
};

}; //namespace Picto
#endif