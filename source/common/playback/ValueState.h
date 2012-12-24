#ifndef _VALUESTATE_H_
#define _VALUESTATE_H_
#include "DataState.h"

namespace Picto {
/*! \brief Extends Data State for Data sources that have a persistant value.
 *	See also Event State
 */
class ValueState : public AutoDataState
{
public:
	ValueState():AutoDataState(){};
	virtual ~ValueState(){};
};

}; //namespace Picto
#endif