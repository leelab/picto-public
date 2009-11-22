#ifndef _PROPERTYCONTAINER_H_
#define _PROPERTYCONTAINER_H_

#include "../common.h"

#include "Property.h"

#include <QtVariantPropertyManager>
#include <QString>

namespace Picto {

class PropertyContainer : public QObject
{
	Q_OBJECT

public:
	PropertyContainer(QString _containerName);

	void addProperty(Property _property);
	QVariant getPropertyValue(QString _propertyName);
	void setPropertyValue(QString _propertyName, QVariant _value);
	void setContainerName(QString _containerName);
	QList<QString> getPropertyList();

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
