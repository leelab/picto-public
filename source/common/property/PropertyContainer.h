#ifndef _PROPERTYCONTAINER_H_
#define _PROPERTYCONTAINER_H_
#include <QtVariantPropertyManager>
#include <QtProperty>
#include <QString>
#include <QSharedPointer>

#include "../common.h"

#include "Property.h"

namespace Picto {

class Property;
/*!	\brief A container for groups of properties.
 *
 *	Since most objects have more than one property, it makes sense to provide a 
 *	container object for grouping the properties of a single object.  This uses the
 *	Qt PropertyBrowser framework to keep track of the properties.  (This will come in
 *	handy when we start building the state machine design GUI.)
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyContainer : public QObject
#else
class PropertyContainer : public QObject
#endif
{
	Q_OBJECT

public:
	static QSharedPointer<PropertyContainer> create(QString _containerName);
	virtual ~PropertyContainer(){};
	void copyProperties(QSharedPointer<PropertyContainer> container2);
	QSharedPointer<Property> addProperty(int _type, QString _identifier, QVariant _value, bool allowMultiple = false);
	QVariant getPropertyValue(QString _identifier, int index=0);
	QString getPropertyName(QString _identifier, int index=0);
	QSharedPointer<Property> setPropertyValue(QString _identifier, QVariant _value, int index=0);
	QSharedPointer<Property> getPropertyFromQtProperty(QtProperty *property);
	void setContainerName(QString _containerName);
	QString getContainerName();
	QStringList getPropertyList();
	QSharedPointer<QtVariantPropertyManager> getPropertyManager(){return propManager_;};
	QHash<QString, QVector<QSharedPointer<Property>>> getProperties(){return properties_;};
	QList<QSharedPointer<Property>> getRuntimeProperties();
	QSharedPointer<Property> getProperty(QString _identifier,int index=0);
	QSharedPointer<Property> getContainerGroupProperty(){return containerGroupItem_;};
	void clear();
signals:
	void signalPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);

private:
	QSharedPointer<QtVariantPropertyManager> propManager_;
	PropertyContainer(QString _containerName);
	QSharedPointer<Property> containerGroupItem_;
	QHash<QString, QVector<QSharedPointer<Property>>> properties_;
	QString containerName_;

private slots:
	void slotPropertyManagerValueChanged(QtProperty * property, const QVariant & value);
};


}; //namespace Picto

#endif
