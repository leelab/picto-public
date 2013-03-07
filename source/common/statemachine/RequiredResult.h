#ifndef _REQUIREDRESULT_H_
#define _REQUIREDRESULT_H_

#include "../common.h"
#include "result.h"

namespace Picto {

/*!	\brief A RequiredResult is a result that is not a transition destination but rather triggered by a StateMachineElement's internal logic
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API RequiredResult : public Result
#else
class RequiredResult : public Result
#endif
{
	Q_OBJECT
public:
	RequiredResult();
	virtual ~RequiredResult(){};
	static QSharedPointer<Asset> Create();

protected:
	virtual bool hasScripts(){return false;};
	virtual bool canHaveScripts(){return false;};
};


}; //namespace Picto

#endif
