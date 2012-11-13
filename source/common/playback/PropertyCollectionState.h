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
class PropertyCollectionState : public QObject, public DataState
{
	Q_OBJECT
public:
	PropertyCollectionState();
	virtual ~PropertyCollectionState();

	QSharedPointer<PropertyReader> getPropertyReader(int propId);

	bool setPropertyValue(double time,qulonglong dataId,int propId,QString value);

signals:
	void propertyChanged(int propId, QString value);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	//Each time this changes, this object updates the corresponding PropertyState in its propLookup table
	//to avoid having to iterate through everything every time something changes.
	//Value changed signal is only emitted on last iteration when moving in reverse.
	virtual void  triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);

private:
	QHash<int,QSharedPointer<PropertyState>> propLookup_;
	bool transmitChangeSig_;

private slots:
	void subPropertyChanged(int propId, QString value);
};

}; //namespace Picto
#endif