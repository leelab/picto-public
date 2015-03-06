#ifndef _NUMERIC_PARAMETER_H_
#define _NUMERIC_PARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*!	\brief A parameter for containing numeric integer values.
 *
 *	We wanted to get rid of this parameter, because it is really just a RangeParameter with less options.  Since old 
 *	Experiments use this Parameter though, the easiest way to support them in newer code without allowing this Parameter
 *	into new designs was to keep it in Picto, but now let it show up in the Designer Toolbox, so that is what we have done.
 *	This Parameter stores integer values.  It does the same thing that the RangeParameter
 *	does except without the set range (ie. The range is just from INT_MIN to INT_MAX).
 *	The Value Property is runtime editable and appears as a numeric "integer only" input widget
 *	in the PropertyFrame.  The numeric value is accessible through the javascript "value" property as:
 *	\code
 		NumericParameterName.value = 0.26;
 		var numVal = NumericParameterName.value;
 	\endcode
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API NumericParameter : public Parameter
#else
class NumericParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the value of the Parameter.*/
	Q_PROPERTY(int value READ getValue WRITE setValue)

public:
	NumericParameter();
	virtual ~NumericParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	/*! \brief Returns the current value of this Parameter.*/
	int getValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	/*! \brief Sets the current value of this Parameter to the input.*/
	void setValue(int val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "IntegerParameter";};
	virtual QString friendlyTypeName(){return "Integer";};

	/*! \brief Since this NumericParameter is just the RangeParameter with less options, but we need to support it for old experiments, we can't just get rid of it.  
	 *	By reimplementing Parameter::getUIGroup() to return an empty string, we cause this Parameter to not show up in the
	 *	Designer Toolbox.  This way no one will be able to add it to new Designs but old Experiments will still work.
	 */
	virtual QString getUIGroup(){return "";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
