#ifndef _PROPERTYCONTAINER_H_
#define _PROPERTYCONTAINER_H_

#include "../common.h"

#include "Property.h"

#include <QtVariantPropertyManager>
#include <QString>
#include <QSharedPointer>

namespace Picto {

class Property;
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
	static QSharedPointer<PropertyContainer> create(QString _containerName);
	QSharedPointer<Property> addProperty(int _type, QString _identifier, QVariant _value);
	QVariant getPropertyValue(QString _identifier, int index=0);
	QString getPropertyName(QString _identifier, int index=0);
	QSharedPointer<Property> setPropertyValue(QString _identifier, QVariant _value, int index=0);
	void setContainerName(QString _containerName);
	QString getContainerName();
	QStringList getPropertyList();

signals:
	void signalPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);

private:
	QSharedPointer<QtVariantPropertyManager> propManager_;
	PropertyContainer(QString _containerName);
	QSharedPointer<Property> containerGroupItem_;
	QMap<QString, QVector<QSharedPointer<Property>>> properties_;

private slots:
	void slotPropertyManagerValueChanged(QtProperty * property, const QVariant & value);
};


}; //namespace Picto

#endif
