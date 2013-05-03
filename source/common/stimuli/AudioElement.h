#ifndef _AUDIOELEMENT_H_
#define _AUDIOELEMENT_H_

#include "../common.h"
#include "../statemachine/UIEnabled.h"

namespace Picto {

/*!	\brief Unimplemented. An audio stimuli
 *
 *	This is an unimplemented class that will eventually contain an audio element.
 */
	struct PICTOLIB_CLASS AudioElement /*: public StimulusElement*/: public UIEnabled
{
public:
	AudioElement();
	virtual ~AudioElement(){};
	static QSharedPointer<Asset> Create();
	virtual QString friendlyTypeName(){return "Audio Element";};
};


}; //namespace Picto

#endif
