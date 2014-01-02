#ifndef _STIMULUSELEMENT_H_
#define _STIMULUSELEMENT_H_
#include <QObject>

#include "../common.h"

namespace Picto {

/*!	\brief NOT USED.  Original documentation appears below.  Should probably be deleted.
 *
 *	\details A base class for all stimulus elements.
 *
 *	This base class for all stimulus elements doesn't actually end up doing anything.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

class StimulusElement
{
public:
	StimulusElement();

	virtual QString friendlyTypeName(){return "Stimulus Element";};
};


}; //namespace Picto

#endif
