#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <QVariant>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/Scriptable.h"

namespace Picto {

/*!	\brief A base class for design variables that can be used as runtime parameters.
 *
 *	We frequently want the ability to change values for experimental tasks just before or during runtime.  This is particularly useful for training
 *	users and necessary even during neural recording experiments for the purpose of eye calibration at the very least.  Now, the way to get Property
 *	widgets to show up during experimental runs is to simply set their container's UIEnabled Property true and make sure that they have some 
 *	Properties for whom setPropertyRuntimeEditable() has been called.  That way, runtime editable Property widgets will show up at runtime and the
 *	values set there will be set to their corresponding Properties' run values whenever those Properties enter scope.  
 *	This can all be done at the level of the Scriptable, so from that perspective there seems to be no need for Parameters.  
 *	The Parameter classes however, defines Scriptables that are specially designed to be convenient for runtime editing.  Unlike the Variable class
 *	Parameters are very specific.  Whereas there is only a single NumberVariable that can take both floating point and integer numbers, there are two
 *	Parameters for the same purpose RangeParameter and DoubleParameter.  
 *
 *	The fact is that at one point we considered moving UIEnabled Properties to the Parameter class itself so that only Parameter objects could show up at runtime in the
 *	PropertyFrame.  We decided not to do this however since it is simply too convenient to be able to do things like clicking UIEnabled on a TargetController
 *	during the design process to quickly tune its various timing values.  While this convenience was left alone, we strongly recommend that only Parameter
 *	objects be used in the final version of an Experiment that is run for a test subject.  This keeps things clean, since typically the operator will only see the
 *	values that should be changeable and not lots of other values that have "come along by default." This means that if, for example, the TargetController fixation time
 *	should be changeable during runtime, we recommend that a Parameter to that effect be exposed and then set to the TargetController in a script at the appropriate time.
 *	If the entire TargetController object were made UIEnabled, a lot of extraneous fields would show up in the runtime UI that would be confusing.
 *
 *	Another feature of the Parameter class is the validation functions that it exposes.  Currently we use the valuesAreValid() function to check if the
 *	current Parameter values are okay and the fixValues() function to bring values back into appropriate ranges.  For example, in a RangeParameter, valuesAreValid() would 
 *	return false if the Value property was lower than the Min Property.  valuesAreValid() would bring the Value Property back to the closer of either Min or Max.
 *	This allows for more robust validation and constriction of values to property ranges during runtime.
 *	\note In practice, there is some issue with the valuesAreValid() / fixValues() system.  In the RangeParameter case for example, when the Value property is set lower than the Min Property,
 *	the value is automatically fixed but this doesn't show up in the Property widgets under certain conditions.  Also in the PropertyFrame, it appears that restrictions on 
 *	moving the Value property outside of the allowable range aren't fedback to the widget value displayed.  This should be looked into.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Parameter : public Scriptable
#else
	class Parameter : public Scriptable
#endif
{
	Q_OBJECT

public:

	Parameter();
	virtual ~Parameter(){};

	virtual QString getUITemplate(){return "Parameter";};
	virtual QString friendlyTypeName(){return "Parameter";};
	virtual QString getUIGroup(){return "Parameters";};

	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

public slots:


protected:
	virtual QString defaultTagName(){return "Parameter";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	bool fixingValues_;
};


}; //namespace Picto

#endif
