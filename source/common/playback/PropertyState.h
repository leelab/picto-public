#ifndef _PROPERTYSTATE_H_
#define _PROPERTYSTATE_H_
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PropertyData;
/*! \brief Stores Property PlaybackData values for use in Playback system.
 */
class PropertyState : public PropertyReader, public DataState
{
	Q_OBJECT
public:
	PropertyState(bool forInitProperties = false);
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);
	void setObsoleteAssets(QHash<int,bool> obsoleteAssetIds){obsoleteAssetIds_ = obsoleteAssetIds;};
signals:
	void propertyChanged(int propId, QString value);

private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	QList<PropertyData> data_;
	QHash<int,bool> obsoleteAssetIds_;
	bool forInitProperties_;
//
//protected:
//	virtual void triggerValueChange(bool reverse,bool last);
//	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
//	virtual void requestNextData(PlaybackIndex currLast,bool backward);
//
//private:
//	int propId_;
};

struct PropertyData
{
	PropertyData(){};
	PropertyData(PlaybackIndex index,int assetid,QString val){index_ = index;assetid_ = assetid;val_ = val;};
	PlaybackIndex index_;
	int assetid_;
	QString val_;
};

}; //namespace Picto
#endif