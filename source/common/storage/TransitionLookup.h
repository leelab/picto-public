#ifndef _TRANSITIONLOOKUP_H_
#define _TRANSITIONLOOKUP_H_

#include "../common.h"
#include "DataUnit.h"
#include "../statemachine/transition.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores a single unit of property data as a path, value, time triplet
 *
 *	The data in the TransitionLookup represents one property data transition.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API TransitionLookup : public DataUnit
#else
class TransitionLookup : public DataUnit
#endif
{
public:
	TransitionLookup();
	
	void clear(){lookupList_.clear();};
	void addTransition(QSharedPointer<Transition> trans);

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	QList<int> lookupList_;

protected:
	void addTransition(int id);

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
