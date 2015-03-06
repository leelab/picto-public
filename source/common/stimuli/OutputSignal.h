#ifndef _OUTPUTSIGNAL_H_
#define _OUTPUTSIGNAL_H_
#include <QSharedPointer>

#include "../common.h"
#include "OutputElement.h"
#include "../statemachine/scriptable.h"

namespace Picto {

/*!	\brief A Base class for OutputElements that output voltage signals.
 *
 *	\details Frequently, an experiment requires some kind of means to interface with outside devices using TTL signals.
 *	This is the purpose of the OutputSignal class.  Descendants of OutputSignal allow the Experimental Design to control
 *	the physical voltage of Ports on the computer running the Director.  In this base class, we implement functionality for
 *	setting the physical port handled by the OutputSignal as well as enabling/disabling the port.  Enabling/disabling
 *	a port sets whether the value set in this OutputSignal will actually physically appear on the port or not.  It is 
 *	analogous to visible/invisible in a VisualElement.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct PICTOLIB_CLASS OutputSignal : public OutputElement
{
	Q_OBJECT
	/*! \brief Sets/gets whether this OutputSignal is enabled (ie. The values set inside the OutputSignal will
	 *	appear on the Port).
	 */
	Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled)
public:
	OutputSignal();
	virtual ~OutputSignal();

	/*! \brief Enables/disables the pins handled by this OutputSignal (ie. The values set inside the OutputSignal will
	 *	appear on the Port).
	 */
	virtual void setEnabled(bool active){propertyContainer_->setPropertyValue("Enabled",active);};
	/*! \brief Returns true if this OutputSignal is enabled (ie. The values set inside the OutputSignal will
	 *	appear on the Port), false otherwise.
	 */
	virtual bool getEnabled(){return propertyContainer_->getPropertyValue("Enabled").toBool();};
	/*! \brief Sets the name of the Port handled by this OutputSignal.
	 *	\details Currently values are BNC0 - default, and PAR0 for BNC and Parallel ports respectively.
	*/
	virtual void setPort(QString port){propertyContainer_->setPropertyValue("Port",port);};
	/*! \brief Returns the name of the Port handled by this OutputSignal
	*/
	virtual QString getPort(){return propertyContainer_->getPropertyValue("Port").toString();};
	/*! \brief Returns the pin number handled by this OutputSignal or -1 if the OutputSignal handles more
	 *	than one pin.
	 */
	virtual int getPin(){return -1;};
	/*! \brief Returns the current integer value of the output Signal if we interpret it as a binary integer.
	 *	\note This depends on the size of the output signal.  If it is 8 bits long, we could get a value from
	 *	0-15, if it is 1 bit long, we could only get 0-1.
	 */
	virtual QVariant getValue(){return QVariant(0);};
	bool getVisibleByUser(bool subject);
	virtual QString getUITemplate(){return "OutputSignal";};
	virtual QString friendlyTypeName(){return "Output Signal";};

protected:

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};

}; //namespace Picto

#endif
