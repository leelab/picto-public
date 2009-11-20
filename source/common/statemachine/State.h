#ifndef _STATE_H_
#define _STATE_H_

#include "../common.h"
#include "../statemachine/StateMachineElement.h"
#include "../controlelements/ControlElement.h"
#include "scene.h"

namespace Picto {

class State : public StateMachineElement
{
public:
	State();

private:
	QSharedPointer<Scene> scene_;
	QList<ControlElement> controlElements_;
};


}; //namespace Picto

#endif
