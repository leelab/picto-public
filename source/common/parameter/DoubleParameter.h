#ifndef _DOUBLEPARAMETER_H_
#define _DOUBLEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A simple parameter for holding a floating point numeric value.
 *
 *	Min and Max Properties are also provided to constrain the numeric value to a set range.
 *	The Value Property is runtime editable and appears as a numeric input widget
 *	in the PropertyFrame.  The numeric value is accessible through the javascript "value" property as:
 *	\code
 		DoubleParameterName.value = 0.26;
 		var numVal = DoubleParameterName.value;
 	\endcode
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API DoubleParameter : public Parameter
#else
class DoubleParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the value of the Parameter.*/
	Q_PROPERTY(double value READ getValue WRITE setValue)

public:
	DoubleParameter();
	virtual ~DoubleParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	double getValue();
	void setValue(double val);

	virtual QString getUITemplate(){return "FloatParameter";};
	virtual QString friendlyTypeName(){return "Float Range";};

	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void checkForPropertyChanges();
};


}; //namespace Picto

#endif
