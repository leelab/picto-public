#ifndef _DIGITALOUTPUT_H_
#define _DIGITALOUTPUT_H_

#include "../common.h"

#include "OutputSignal.h"

namespace Picto {

/*!	\brief A circle graphic.
 *
 *	A DigitalOutput has the following properties:
 *	- Position: The position of the top-left corner of the box containing the circle (in screen coordinates)
 *	- Radius: Duh.
 *	- Color: The color of the circle
 */

class PICTOLIB_CLASS DigitalOutput : public OutputSignal
{
	Q_OBJECT
	Q_PROPERTY(bool output READ getOutput WRITE setOutput)
public:
	DigitalOutput();

	static QSharedPointer<Asset> Create();
	static const QString type;
	
	void setOutput(bool active){propertyContainer_->setPropertyValue("Output",active);};
	bool getOutput(){return propertyContainer_->getPropertyValue("Output").toBool();};
	virtual int getVoltage(){return int(getOutput())*5.0;};


protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
