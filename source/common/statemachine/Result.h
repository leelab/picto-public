/*!	\brief A Result is a state machine element that determines what is returned
 */

#ifndef _RESULT_H_
#define _RESULT_H_

#include "../common.h"
#include "StateMachineElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Result : public StateMachineElement
#else
class Result : public StateMachineElement
#endif
{
public:
	Result();

	QString run();
	void setResultValue(QString value) { resultValue_ = value; };

private:	
	QString resultValue_;	//e.g. "Success", "Broke Fixation", etc
};


}; //namespace Picto

#endif
