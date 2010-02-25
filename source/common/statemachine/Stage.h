/*!	\brief a Stage is part of a trial */
#ifndef _STAGE_H_
#define _STAGE_H_

#include "common.h"
#include "../statemachine/StateMachineElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Stage : public StateMachineElement
#else
class Stage : public StateMachineElement
#endif
{
public:
	Stage();
	QString run();
};


}; //namespace Picto

#endif
