/*!	\brief A Trial is part of a Task, and contains Stages */
#ifndef _TRIAL_H_
#define _TRIAL_H_

#include "common.h"
#include "../statemachine/StateMachineElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Trial : public StateMachineElement
#else
class Trial : public StateMachineElement
#endif
{
public:
	Trial();
	QString run();

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif