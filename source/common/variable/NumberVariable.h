#ifndef _NumberVariable_H_
#define _NumberVariable_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A simple variable that is used for storing numeric values.
 *	\details The name pretty much says it all.  This is like a javascript var.  It holds
 *	a number while it is in scope.  The number can be read/set using the "value" script property.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API NumberVariable : public Variable
#else
class NumberVariable : public Variable
#endif
{
	Q_OBJECT
	/*! \brief A script property used to read and write this NumberVariable's value.*/
	Q_PROPERTY(double value READ getValue WRITE setValue)

public:
	NumberVariable();
	virtual ~NumberVariable(){};

	static QSharedPointer<Asset> Create();
	/*! \brief Returns this NumberVariable's value.*/
	double getValue(){return propertyContainer_->getPropertyValue("Value").toDouble();};
	/*! \brief Sets this NumberVariable's value.*/
	void setValue(double val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "NumberVariable";};
	virtual QString friendlyTypeName(){return "Number Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
