#ifndef _PROPERTYCONTAINER_H_
#define _PROPERTYCONTAINER_H_

#include "../common.h"

#include "Property.h"

#include <QtVariantPropertyManager>
#include <QString>

namespace Picto {

/*!	\brief A container for groups of properties.
 *
 *	Since most objects have more than one property, it makes sense to provide a 
 *	container object for grouping the properties of a single object.  This uses the
 *	Qt PropertyBrowser framework to keep track of the properties.  (This will come in
 *	handy when we start building the state machine design GUI.)
 */
class PropertyContainer : public QObject
{
	Q_OBJECT

public:
	PropertyContainer(QString _containerName);

	void addProperty(Property _property);
	QVariant getPropertyValue(QString _propertyName);
	void setPropertyValue(QString _propertyName, QVariant _value);
	void setContainerName(QString _containerName);
	QString getContainerName();
	QStringList getPropertyList();

signals:
	void signalPropertyValueChanged(QString propertyName, QVariant propertyValue);

private:
	QtVariantPropertyManager variantManager_;
	QtVariantProperty * containerGroupItem_;
	QMap<QString, QtVariantProperty *> properties_;

private slots:
	void slotPropertyManagerValueChanged(QtProperty * property, const QVariant & value);
};


}; //namespace Picto

#endif
