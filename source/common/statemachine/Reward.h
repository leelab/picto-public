#ifndef _REWARD_H_
#define _REWARD_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A Reward is a state machine element that determines what is returned
 *	
 *	A reward is typically the final stop of a state machine. If a reward is added to a StateMachine
 *	it acts as a StateMachineElement; you can transition to it, and call run on it.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Reward : public StateMachineElement
#else
class Reward : public StateMachineElement
#endif
{
public:
	Reward();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new Reward());};

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);
	//! \TODO Add rewarding options here...

	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
