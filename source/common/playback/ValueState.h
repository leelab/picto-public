#ifndef _VALUESTATE_H_
#define _VALUESTATE_H_
#include "DataState.h"

namespace Picto {
/*! \brief A lot has changed since the original Picto Playback Design, this class was part of an attempt
 *	to make code of different DataState types very reusable.  In the end, more code reusability led to
 *	less efficiency in terms of timing and memory space and we opted for a more tailored approach
 *	with each of the DataState classes doing a lot more of its own work to handle the DataState interface
 *	in the most efficient way possible.
 *
 *	We should probably remove this code. But the original comments appear below for reference.
 *
 *	Extends Data State for Data sources that have a persistant value.
 *	\sa EventState
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ValueState : public AutoDataState
{
public:
	ValueState():AutoDataState(){};
	virtual ~ValueState(){};
};

}; //namespace Picto
#endif