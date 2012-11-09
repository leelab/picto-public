#ifndef _PROPERTYCOLLECTIONSTATE_H_
#define _PROPERTYCOLLECTIONSTATE_H_
#include <QObject>
#include "DataState.h"
#include "PropertyState.h"

namespace Picto {
/*! \brief Extends Data State for a collection of all experimental properties.
 *	Rather than maintaining hundreds of PropertyStates and using them in the
 *	same way other DataStates are used, we use this collection object as an
 *	interface to the collection of properties.  This allows us to optimize
 *	moving through time by watching which properties change as time goes by
 *	and updating the current index pointer of those properties only.  When
 *	the system wants to know the history (or future) of a particular PropertyState
 *	it can use the corresponding PropertyReader, but this object handles all 
 *	changes to the current index of the underlying property states.
 */
struct PropertyCollectionData;
class PropertyCollectionState : public QObject, public DataState<qulonglong>
{
	Q_OBJECT
public:
	PropertyCollectionState();
	virtual ~PropertyCollectionState();

	QSharedPointer<PropertyReader> getPropertyReader(int propId);

	bool setPropertyValue(double time,qulonglong dataId,int propId,QString value);

signals:
	void propertyChanged(int propId, QString value);

protected:
	//Each time this changes, this object updates the corresponding PropertyState in its propLookup table
	//to avoid having to iterate through everything every time something changes.
	//Value changed signal is only emitted on last iteration when moving in reverse.
	virtual void  triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(qulonglong index);
	virtual void requestMoreDataByTime(double time);

private:
	QHash<int,QSharedPointer<PropertyState>> propLookup_;
	bool transmitChangeSig_;

private slots:
	void subPropertyChanged(int propId, QString value);
};

struct PropertyCollectionData : public IndexedData<qulonglong>
{
	PropertyCollectionData(double time,qulonglong index,int propId):IndexedData(time,index){propId_ = propId;};
	int propId_;
};

}; //namespace Picto
#endif