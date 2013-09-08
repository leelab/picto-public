#ifndef _BINARYDATAOUTPUT_H_
#define _BINARYDATAOUTPUT_H_

#include "../common.h"

#include "OutputSignal.h"

namespace Picto {

/*!	\brief A circle graphic.
 *
 *	A BinaryDataOutput has the following properties:
 *	Widget to control the output of a digital port from the director.
 */

class PICTOLIB_CLASS BinaryDataOutput : public OutputSignal
{
	Q_OBJECT
	Q_PROPERTY(int value READ getBinValue WRITE setBinValue)
public:
	BinaryDataOutput();

	static QSharedPointer<Asset> Create();
	static const QString type;
	
	void setBinValue(int value){propertyContainer_->setPropertyValue("Value",value);};
	int getBinValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	void setPin(int pin){propertyContainer_->setPropertyValue("Pin",pin);};
	virtual int getPin(){return -1;};
	virtual QVariant getValue(){return QVariant(getBinValue());};

	virtual QString getUITemplate(){return "BinarySignal";};
	virtual QString friendlyTypeName(){return "Binary Data Output";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
