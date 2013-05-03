#ifndef _LogicResult_H_
#define _LogicResult_H_

#include "../common.h"
#include "result.h"

namespace Picto {

/*!	\brief A LogicResult is a result that is not a transition destination but rather triggered by a StateMachineElement's internal logic
 *	
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

protected:
	virtual bool hasScripts(){return false;};
	virtual bool canHaveScripts(){return false;};
};


}; //namespace Picto

#endif
