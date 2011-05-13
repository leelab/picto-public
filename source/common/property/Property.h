#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "../common.h"
#include "../storage/datastore.h"

#include <QtVariantPropertyManager>
#include <QVariant>
#include <QString>
#include <QMap>
#include <QScriptEngine>
#include <QXmlStreamWriter>
#include <QSharedPointer>

namespace Picto {
class PropertyContainer;
class DataStore;
/*!	\brief Describes the property of an object
 *
 *	Many objects in Picto have properties (e.g. a circle has properties of color 
 *	radius, and location).  The Property object allows us to assign properties to 
 *	objects.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Property : public DataStore
#else
class Property : public DataStore
#endif
{
	Q_OBJECT
public:
	virtual ~Property();
	int type();
	QVariant value();
	virtual QString valueString();
	QString name();
	virtual void setValue(QVariant _value);
	void addAttribute(QString _attributeName, QVariant _attributeValue);

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	Property(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	QSharedPointer<QtVariantProperty> variantProp_;

	//Fields used by subclases that need serialization attributes
	void AddSerializationAttribute(QString name);
	void SetSerializationAttributeValue(QString name, QVariant _value);
	QVariant GetSerializationAttributeValue(QString name);
	virtual void UpdateSerializationAttributesFromValue();
	virtual void SetValueFromDeserializedData(QVariant _value);
	QMap<QString,QVariant> serializationAttributes_;

private:
	friend class PropertyContainer;
	void addSubProperty(QSharedPointer<Property> prop);
	void setName(QString name);
	QString tagName_;
	QSharedPointer<QtVariantPropertyManager> manager_;

private slots:
	void valueChanged(QtProperty *property, const QVariant &val);
    void attributeChanged(QtProperty *property,
                const QString &attribute, const QVariant &val);
};


}; //namespace Picto

#endif
