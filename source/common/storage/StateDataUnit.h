#ifndef _STATEDATAUNIT_H_
#define _STATEDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"
#include "../statemachine/transition.h"

#include <QString>

namespace Picto {

/*!	\brief Stores data about a particular Transition traversal that occured during a Task run 
 *	for transmission over the network.
 *
 *	\details Objects of this class store the AssetId of a Transition that was traversed along
 *	with the DataId of the Frame that was presented following that traversal.  This information
 *	can later be used to reproduce the precise path through the StateMachine that was taken
 *	during a given session.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API StateDataUnit : public DataUnit
#else
class StateDataUnit : public DataUnit
#endif
{
public:
	StateDataUnit();

	void setTransition(QSharedPointer<Transition> transition);
	void setTransition(int id);

	/*! \brief Sets the DataId of the Frame that was presented following the Transition traversal
	 *	contained in this object.*/
	void setActionFrame(qulonglong frameId){actionFrame_ = frameId;};

	/*! \brief Returns the Transition AssetId stored in this object.*/
	int	getTransitionID(){return id_;};
	/*! \brief Returns the DataId of the Frame that was presented following the Transition traversal
	 *	contained in this object.*/
	qulonglong getActionFrame(){return actionFrame_;};

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	qulonglong actionFrame_;
	int id_;
};


}; //namespace Picto

#endif
