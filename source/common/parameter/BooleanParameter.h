#ifndef _BOOLEANPARAMETER_H_
#define _BOOLEANPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A simple parameter for holding a boolean value.
 *
 *	The Value Property is runtime editable and appears as a checkbox widget
 *	in the PropertyFrame.  The boolean value is accessible through the javascript "value" property as:
 *	\code
 		BooleanParameterName.value = false;
 		var boolVal = BooleanParameterName.value;
 	\endcode
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API BooleanParameter : public Parameter
#else
class BooleanParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the stored boolean value.*/
	Q_PROPERTY(bool value READ getValue WRITE setValue)

public:
	BooleanParameter();
	virtual ~BooleanParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	/*! \brief Returns the stored boolean value.*/
	bool getValue(){return propertyContainer_->getPropertyValue("Value").toBool();};
	/*! \brief Sets the stored boolean value.*/
	void setValue(bool val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "BooleanParameter";};
	virtual QString friendlyTypeName(){return "Boolean";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
