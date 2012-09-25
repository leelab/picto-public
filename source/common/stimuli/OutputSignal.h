#ifndef _OUTPUTSIGNAL_H_
#define _OUTPUTSIGNAL_H_
#include <QSharedPointer>

#include "../common.h"

#include "../statemachine/scriptable.h"

namespace Picto {

/*!	\brief A Base class for all Digital Elements
 *
 */
struct PICTOLIB_CLASS OutputSignal : /*public StimulusElement,*/ public Scriptable
{
	Q_OBJECT
	Q_PROPERTY(int port READ getPort WRITE setPort)
	Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled)
public:
	OutputSignal();
	virtual ~OutputSignal();

	virtual void setEnabled(bool active){propertyContainer_->setPropertyValue("Enabled",active);};
	virtual bool getEnabled(){return propertyContainer_->getPropertyValue("Enabled").toBool();};
	virtual void setPort(int port){propertyContainer_->setPropertyValue("Port",port);};
	virtual int getPort(){return propertyContainer_->getPropertyValue("Port").toInt();};
	virtual int getVoltage() = 0;
	bool getVisibleByUser(bool subject);
	virtual QString getUITemplate(){return "OutputSignal";};
	virtual QString assetType(){return "OutputSignal";};

protected:

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};

}; //namespace Picto

#endif
