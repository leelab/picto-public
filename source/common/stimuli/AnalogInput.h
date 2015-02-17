#ifndef _ANALOGINPUT_H_
#define _ANALOGINPUT_H_

#include "../common.h"

#include "InputSignal.h"

namespace Picto {

	/*!	\brief An InputSignal that handles the value of single pins of an entire port.
	*	\details The pin's voltage value is set as a bolean.
	*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2015
	*/
	class PICTOLIB_CLASS AnalogInput : public InputSignal
	{
		Q_OBJECT
			/*! \brief Gets boolean the value on the pin handled by this object.*/
			Q_PROPERTY(double value READ getPinValue)
			/*! \brief Sets/gets the index of the pin handled by this object.*/
			Q_PROPERTY(int pin READ getPin WRITE setPin)
	public:
		AnalogInput();
		virtual ~AnalogInput() {};

		static QSharedPointer<Asset> Create();
		static const QString type;

		/*! \brief Sets the value on the pin handled by this object to the input boolean.*/
		void setPinValue(double value){ propertyContainer_->setPropertyValue("Value", value); };
		/*! \brief Returns the value on the pin handled by this object.*/
		double getPinValue(){ return propertyContainer_->getPropertyValue("Value").toDouble(); };

		/*! \brief Sets the index of the pin handled by this object.*/
		void setPin(int pin){ propertyContainer_->setPropertyValue("Pin", pin); };
		/*! \brief Returns the index of the pin handled by this object within its port.*/
		virtual int getPin(){ return propertyContainer_->getPropertyValue("Pin").toInt(); };
		virtual void setValue(double value){ setPinValue(value); };

		virtual QString friendlyTypeName(){ return "Analog Input"; };
	protected:
		virtual void postDeserialize();
		virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	};


}; //namespace Picto

#endif
