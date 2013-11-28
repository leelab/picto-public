#ifndef _SlaveEvent_H_
#define _SlaveEvent_H_

#include "QString"

namespace Picto {

/*! \brief Holds information about an experimental state change event.  Used by SlaveExperimentDriver.
 *	\details Property value, Property init value and Transition activation event times are included.
 *	Each SlaveEvent has a type, an ID representing the affected Property or Transition's Asset ID, and 
 *	a string value which is not used by TRANS_ACTIVATED events but holds a Property's value/initValue for
 *	PROP_VAL_CHANGE and INIT_PROP_VAL_CHANGE events.
 *	\sa SlaveEventQueue, SlaveExperimentDriver
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

class SlaveEvent
{
public:
	/*! \brief The type of this SlaveEvent.*/
	enum EventType	{	
						INVALID,				//!< EventType is invalid. 
						PROP_VAL_CHANGE,		//!< A Property object's run value changed.
						INIT_PROP_VAL_CHANGE,	//!< A Property object's init value changed.
						TRANS_ACTIVATED			//!< A Transition was traversed.
					};
	SlaveEvent();
	~SlaveEvent(){};

	void setAsProp(const int& assetId, const QString& val);
	void setAsInitProp(const int& assetId, const QString& val);
	void setAsTrans(const int& assetId);
	EventType type;								//!< The type of event that occured.
	int id;										//!< The Asset ID of the Property whose value/initValue changed or of the Transition that was traversed.
	QString value;								//!< For Property events, the new value of the value/initValue.
};


}; //namespace Picto

#endif
