#ifndef _DIGITALOUTPUT_H_
#define _DIGITALOUTPUT_H_

#include "../common.h"

#include "OutputSignal.h"

namespace Picto {

/*!	\brief An OutputSignal that handles the value of single pins of an entire port.
 *	\details The pin's voltage value is set as a bolean.
 *	\note Using a BinaryDataOutput object and a DigitalOutput object with the same port
 *	is undefined.  We might want to check for this at some point in validateObject.
 *	More than one DigitalOutput can be used with the same port so long as eash one 
 *	handles a different pin.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS DigitalOutput : public OutputSignal
{
	Q_OBJECT
	/*! \brief Sets/gets boolean the value on the pin handled by this object.*/
	Q_PROPERTY(bool value READ getBinValue WRITE setBinValue)
	/*! \brief Sets/gets the index of the pin handled by this object.*/
	Q_PROPERTY(int pin READ getPin WRITE setPin)
public:
	DigitalOutput();
	virtual ~DigitalOutput() {};

	static QSharedPointer<Asset> Create();
	static const QString type;
	
	/*! \brief Sets the value on the pin handled by this object to the input boolean.*/
	void setBinValue(bool active){propertyContainer_->setPropertyValue("Value",active);};
	/*! \brief Returns the value on the pin handled by this object.  True or false.*/
	bool getBinValue(){return propertyContainer_->getPropertyValue("Value").toBool();};
	/*! \brief Sets the index of the pin handled by this object.*/
	void setPin(int pin){propertyContainer_->setPropertyValue("Pin",pin);};
	/*! \brief Returns the index of the pin handled by this object within its port.*/
	virtual int getPin(){return propertyContainer_->getPropertyValue("Pin").toInt();};
	virtual QVariant getValue(){return QVariant(int(getBinValue()));};

	virtual QString friendlyTypeName(){return "Digital Output";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
