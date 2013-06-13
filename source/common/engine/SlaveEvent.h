#ifndef _SlaveEvent_H_
#define _SlaveEvent_H_

#include "QString"

namespace Picto {

/*! SlaveEvent
 *
 *	Holds information about an experimental state change event.  Used by SlaveExperimentDriver.
 */

class SlaveEvent
{
public:
	enum EventType	{	
						INVALID,
						PROP_VAL_CHANGE,
						INIT_PROP_VAL_CHANGE,
						TRANS_ACTIVATED
					};
	SlaveEvent();
	~SlaveEvent(){};

	void setAsProp(const int& assetId, const QString& val);
	void setAsInitProp(const int& assetId, const QString& val);
	void setAsTrans(const int& assetId);
	EventType type;
	int id;
	QString value;
};


}; //namespace Picto

#endif
