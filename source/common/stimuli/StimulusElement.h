#ifndef _STIMULUSELEMENT_H_
#define _STIMULUSELEMENT_H_
#include <QObject>

#include "../common.h"

namespace Picto {

/*!	\brief A base class for all stimulus elements.
 *
 *	This base class for all stimulus elements doesn't actually end up doing anything.
 *	I suppose we could delete it...  Or perhpas, we should move the DataStore 
 *	inheritance to here...
 */

class StimulusElement
{
public:
	StimulusElement();

	virtual QString friendlyTypeName(){return "Stimulus Element";};
};


}; //namespace Picto

#endif
