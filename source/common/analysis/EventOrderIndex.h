#ifndef _EVENT_ORDER_INDEX_H_
#define _EVENT_ORDER_INDEX_H_

#include <QVariant>
#include <QScriptValue>
#include <QSqlDatabase>
#include "../statemachine/UIEnabled.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API EventOrderIndex
#else
class EventOrderIndex
#endif
{
public:
	enum IDSource { BEHAVIORAL,NEURAL };
	EventOrderIndex();
	EventOrderIndex(double time);
	EventOrderIndex(double time, qulonglong dataId, IDSource idSource);
	virtual ~EventOrderIndex();
	
	//Variables
	qulonglong dataId_;
	IDSource idSource_;
	double time_;

	inline bool operator>(const EventOrderIndex &otherIndex) const 
	{
		if((dataId_>0)&&(otherIndex.dataId_>0)&&(idSource_ == otherIndex.idSource_))
			return dataId_>otherIndex.dataId_;
		else
			return time_>otherIndex.time_;
	}
	inline bool operator>=(const EventOrderIndex &otherIndex) const
	{
		if((dataId_>0)&&(otherIndex.dataId_>0)&&(idSource_ == otherIndex.idSource_))
			return dataId_>=otherIndex.dataId_;
		else
			return time_>=otherIndex.time_;
	}
	inline bool operator<(const EventOrderIndex &otherIndex) const 
	{
		if((dataId_>0)&&(otherIndex.dataId_>0)&&(idSource_ == otherIndex.idSource_))
			return dataId_<otherIndex.dataId_;
		else
			return time_<otherIndex.time_;
	}
	inline bool operator<=(const EventOrderIndex &otherIndex) const 
	{
		if((dataId_>0)&&(otherIndex.dataId_>0)&&(idSource_ == otherIndex.idSource_))
			return dataId_<=otherIndex.dataId_;
		else
			return time_<=otherIndex.time_;
	}
	inline bool operator==(const EventOrderIndex &otherIndex) const 
	{
		if((dataId_>0)&&(otherIndex.dataId_>0)&&(idSource_ == otherIndex.idSource_))
			return dataId_==otherIndex.dataId_;
		else
			return time_==otherIndex.time_;
	}
	inline bool operator!=(const EventOrderIndex &otherIndex) const 
	{
		if((dataId_>0)&&(otherIndex.dataId_>0)&&(idSource_ == otherIndex.idSource_))
			return dataId_!=otherIndex.dataId_;
		else
			return time_!=otherIndex.time_;
	}
	inline EventOrderIndex& operator=(const EventOrderIndex &otherIndex)
	{
		time_ = otherIndex.time_;
		dataId_ = otherIndex.dataId_;
		idSource_ = otherIndex.idSource_;
		return *this;
	};
	inline bool operator>(const double otherValue) const {return time_ > otherValue;};
	inline bool operator>=(const double otherValue) const {return time_ >= otherValue;};
	inline bool operator<(const double otherValue) const {return time_ < otherValue;};
	inline bool operator<=(const double otherValue) const {return time_ <= otherValue;};
	inline bool operator==(const double otherValue) const {return time_ == otherValue;};
	inline bool operator!=(const double otherValue) const {return time_ != otherValue;};
	inline EventOrderIndex& operator=(const double otherValue) {time_ = otherValue;return *this;};
	bool isValid(){return time_ > INT_MIN;};
};
}; //namespace Picto
#endif