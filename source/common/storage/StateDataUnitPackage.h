#ifndef _STATEDATAUNITPACKAGE_H_
#define _STATEDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "StateDataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores a list of StateDataUnit objects representing StateMachine Transitions between 
 *	one frame and the next.
 *	\details Objects of this class store the entire StateMachine traversal that occured during the
 *	course of a single frame.  The traversal is stored as a list of StateDataUnit objects each of
 *	which contains a Transition AssetId.  These StateDataUnit objects are stored with the DataId
 *	of the Frame that followed them, and since they each have their own DataId, we can follow
 *	the order of Transition traversals accordingly.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API StateDataUnitPackage : public DataUnit
#else
class StateDataUnitPackage : public DataUnit
#endif
{
public:

	StateDataUnitPackage();

	void addTransition(QSharedPointer<Transition> transition);
	void addTransition(int id);
	void setActionFrame(qulonglong frameId);
	/*! \brief Returns the number of StateDataUnit objects in this package's list.*/
	int length() { return data_.length(); }
	/*! \brief Removes all StateDataUnit objects from this package's list.*/
	void clear() { data_.clear(); };
	/*! \brief Returns the first StateDataUnit in this package's list then removes it from the list.*/
	QSharedPointer<StateDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	/*! \brief Returns the last StateDataUnit in this package's list then removes it from the list.*/
	QSharedPointer<StateDataUnit> takeLastDataPoint() { return data_.takeLast(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<StateDataUnit>> data_;
};


}; //namespace Picto

#endif
