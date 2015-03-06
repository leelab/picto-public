#ifndef _RANGEPARAMETER_H_
#define _RANGEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*!	\brief A Parameter that holds integer values within a limited range
 *
 *	\details The range is between the Min and Max Property values (inclusive).  There is also
 *	a legacy Property called Increment.  This Property has been set invisible for new versions
 *	of Picto since it is really just confusing and not too useful; however, it has been left 
 *	around to support older designs.  It forces the Value Property to be a precise number of
 *	Increments from the Min value if used.  If set to zero (the default), it is not used.
 *
 *	Currently, the widget representing the RangeParameter value in the PropertyFrame in the TestViewer
 *	and RemoteViewer doesn't appear to be limiting values to be within the Min-Max range.  We would
 *	have expected this to work automatically when fixValues() brings the value back to the allowed
 *	range, but there appears to be some issue. This shoul be looked into.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RangeParameter : public Parameter
#else
class RangeParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the value of the Parameter.*/
	Q_PROPERTY(int value READ getValue WRITE setValue)

public:
	RangeParameter();
	virtual ~RangeParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	/*! \brief Returns the current value of this Parameter.*/
	int getValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	/*! \brief Sets the current value of this Parameter to the input.*/
	void setValue(int val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "IntegerParameter";};
	virtual QString friendlyTypeName(){return "Integer Range";};

	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
