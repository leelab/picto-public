#ifndef _OUTPUTSIGNAL_H_
#define _OUTPUTSIGNAL_H_
#include <QSharedPointer>

#include "../common.h"
#include "OutputElement.h"
#include "../statemachine/scriptable.h"

namespace Picto {

/*!	\brief A Base class for all Digital Elements
 *
 */
struct PICTOLIB_CLASS OutputSignal : public OutputElement
{
	Q_OBJECT
	Q_PROPERTY(QString port READ getPort WRITE setPort)
	Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled)
public:
	OutputSignal();
	virtual ~OutputSignal();

	virtual void setEnabled(bool active){propertyContainer_->setPropertyValue("Enabled",active);};
	virtual bool getEnabled(){return propertyContainer_->getPropertyValue("Enabled").toBool();};
	virtual void setPort(QString port){propertyContainer_->setPropertyValue("Port",port);};
	virtual QString getPort(){return propertyContainer_->getPropertyValue("Port").toString();};
	//Should return -1 if this OutputSignal handles the entire port.  The pin number that it handles
	//otherwise.
	virtual int getPin(){return -1;};
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
