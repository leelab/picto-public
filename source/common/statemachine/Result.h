#ifndef _RESULT_H_
#define _RESULT_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A Result is a state machine element that determines what is returned
 *	
 *	Results are used in two disparate manners.  If a result is added to a StateMachine
 *	it acts as a StateMachineElement; you can transition to it, and call run on it.
 *	However, results are also used within StateMachineElements.  When addResult is
 *	called, on a StateMachineElement, the "name" of the result is added to the 
 *	element's result list.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Result : public StateMachineElement
#else
class Result : public StateMachineElement
#endif
{
public:
	Result();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);
	//! \TODO Add rewarding options here...

	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
