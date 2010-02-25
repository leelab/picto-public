#ifndef _STATE_H_
#define _STATE_H_

#include "../common.h"
#include "../statemachine/StateMachineElement.h"
#include "../controlelements/ControlElement.h"
#include "scene.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API State : public StateMachineElement
#else
class State : public StateMachineElement
#endif
{
public:
	State();
	QString run();

private:
	QSharedPointer<Scene> scene_;
	QList<ControlElement> controlElements_;
};


}; //namespace Picto

#endif
