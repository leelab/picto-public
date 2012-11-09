#ifndef _EVENTSTATE_H_
#define _EVENTSTATE_H_
#include "DataState.h"

namespace Picto {
/*! \brief Extends Data State for Data sources that represent discontinuous events
 *	having no persistant values.
 *	See also Value State
 */
template <class I>// <type of data index>
class EventState : public DataState<I>
{
public:
	EventState():DataState(false){};
	virtual ~EventState(){};
};

}; //namespace Picto
#endif