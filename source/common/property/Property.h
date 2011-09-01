#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "../common.h"
#include "../storage/Asset.h"

#include <QtVariantPropertyManager>
#include <QVariant>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>
#include <QSharedPointer>

namespace Picto {
class PropertyContainer;
/*!	\brief Describes the property of an object
 *
 *	Many objects in Picto have properties (e.g. a circle has properties of color 
 *	radius, and location).  The Property object allows us to assign properties to 
 *	objects.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Property : public Asset
#else
class Property : public Asset
#endif
{
	Q_OBJECT
public:
	virtual ~Property();
	int type();
	//QVariant value();
	virtual QString valueString();
	QString getName();
	//virtual void setValue(QVariant _value);
	void setAttribute(QString _attributeName, QVariant _attributeValue);
	QVariant attributeValue(QString _attributeName);
	QSharedPointer<QtVariantProperty> getVariantProperty(){return variantProp_;};
	QVariant value();
	virtual void setValue(QVariant _value);
	//Set this property as changeable during runtime.
	void setRuntimeEditable(bool enabled = true){runtimeEnabled_ = enabled;};
	bool isRuntimeEnabled(){return runtimeEnabled_;};
	int getIndex(){return index_;};
	void setIndex(int index){index_ = index;};

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader, bool validate);

	virtual QString identifier(){return tagName_;};
	virtual QString assetType(){return "Property";};

signals:
	void valueChanged(QSharedPointer<Property> changedProp);
public slots:
	void setValueFromProp(QSharedPointer<Property> prop);

protected:
	Property(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	QSharedPointer<QtVariantProperty> variantProp_;

	//Fields used by subclases that need serialization attributes
	void AddSerializationAttribute(QString name);
	void SetSerializationAttributeValue(QString name, QVariant _value);
	QVariant GetSerializationAttributeValue(QString name);
	virtual void UpdateSerializationAttributesFromValue();
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QMap<QString,QVariant> serializationAttributes_;

private:
	friend class PropertyContainer;
	void addSubProperty(QSharedPointer<Property> prop);
	void setName(QString name);
	int index_;
	QString tagName_;
	QString tagText_;
	QString typeVal_; // In cases where a Asset Factory used a type attribute to choose between types, a type that we don't use but need to write out would be in the tag.
	QSharedPointer<QtVariantPropertyManager> manager_;
	bool scriptEditable_;
	bool runtimeEnabled_;

private slots:
	void valueChanged(QtProperty *property, const QVariant &val);
    void attributeChanged(QtProperty *property,
                const QString &attribute, const QVariant &val);
};


}; //namespace Picto

#endif
