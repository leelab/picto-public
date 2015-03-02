#ifndef _INPUTSIGNAL_H_
#define _INPUTSIGNAL_H_
#include <QSharedPointer>

#include "../common.h"
#include "OutputElement.h"
#include "../statemachine/scriptable.h"

namespace Picto {

/*!	\brief A Base class for InputElements that read analog voltage signals.
*
*	\details Frequently, an experiment requires some kind of means to interface with outside devices using TTL signals.
*	This is the purpose of the InputSignal class.  Descendants of InputSignal allow the Experimental Design to read
*	the physical voltage of Ports on the computer running the Director.  In this base class, we implement functionality for
*	setting the physical port handled by the InputSignal as well as enabling/disabling the port.  Enabling/disabling
*	a port sets whether the value set in this InputSignal will actually update based on the port or not.  It is
*	analogous to visible/invisible in a VisualElement.
*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
struct PICTOLIB_CLASS InputSignal : public OutputElement
{
	Q_OBJECT

public:
	InputSignal();
	virtual ~InputSignal();

	static const QString type;

	/*! \brief Returns the pin number handled by this InputSignal.
	*/
	virtual int getPin(){ return -1; };
	/*! \brief Sets current integer value of the Input Signal.
	*/
	virtual void setValue(double){};
	bool getVisibleByUser(bool subject);
	virtual QString getUITemplate(){ return "InputSignal"; };
	virtual QString friendlyTypeName(){ return "Input Signal"; };

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};
};

#endif