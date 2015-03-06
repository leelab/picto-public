#ifndef _LogicResult_H_
#define _LogicResult_H_

#include "../common.h"
#include "result.h"

namespace Picto {

/*!	\brief A LogicResult is a Result that is not a transition destination but rather triggered by a StateMachineElement's internal logic
 *	
 *	\details This is the class used for low level Results that live beneath the Design's StateMachine.  For example, in the case of a
 *	TargetController, the C++ logic itself decides which Results should be triggered.	
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LogicResult : public Result
#else
class LogicResult : public Result
#endif
{
	Q_OBJECT
public:
	LogicResult();
	virtual ~LogicResult(){};
	static QSharedPointer<Asset> Create();
	virtual QString friendlyTypeName(){return "Logic Result";};
};


}; //namespace Picto

#endif
