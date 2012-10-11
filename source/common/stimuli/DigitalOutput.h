#ifndef _DIGITALOUTPUT_H_
#define _DIGITALOUTPUT_H_

#include "../common.h"

#include "OutputSignal.h"

namespace Picto {

/*!	\brief A circle graphic.
 *
 *	A DigitalOutput has the following properties:
 *  Widget to control the output of a single digital pin from the director.
 */

class PICTOLIB_CLASS DigitalOutput : public OutputSignal
{
	Q_OBJECT
	Q_PROPERTY(bool value READ getBinValue WRITE setBinValue)
	Q_PROPERTY(int pin READ getPin WRITE setPin)
public:
	DigitalOutput();

	static QSharedPointer<Asset> Create();
	static const QString type;
	
	void setBinValue(bool active){propertyContainer_->setPropertyValue("Value",active);};
	bool getBinValue(){return propertyContainer_->getPropertyValue("Value").toBool();};
	void setPin(int pin){propertyContainer_->setPropertyValue("Pin",pin);};
	virtual int getPin(){return propertyContainer_->getPropertyValue("Pin").toInt();};
	virtual QVariant getValue(){return QVariant(int(getBinValue()));};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
