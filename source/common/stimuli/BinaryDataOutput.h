#ifndef _BINARYDATAOUTPUT_H_
#define _BINARYDATAOUTPUT_H_

#include "../common.h"

#include "OutputSignal.h"

namespace Picto {

/*!	\brief An OutputSignal that handles the value of multiple pins over an entire port.
 *	\details Output values are set as an integer.  The integer is intepreted
 *	as a binary value and written to the output port accordingly.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS BinaryDataOutput : public OutputSignal
{
	Q_OBJECT
	/*! \brief Gets/sets the value on the output port as an integer in binary form.*/
	Q_PROPERTY(int value READ getBinValue WRITE setBinValue)
public:
	BinaryDataOutput();

	static QSharedPointer<Asset> Create();
	static const QString type;
	
	/*! \brief Sets the value on the output port to the binary form of the input integer.*/
	void setBinValue(int value){propertyContainer_->setPropertyValue("Value",value);};
	/*! \brief Returns the value on the output port when interpreted as the binary form of an integer.*/
	int getBinValue(){return propertyContainer_->getPropertyValue("Value").toInt();};

	virtual QVariant getValue(){return QVariant(getBinValue());};

	virtual QString getUITemplate(){return "BinarySignal";};
	virtual QString friendlyTypeName(){return "Binary Data Output";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
