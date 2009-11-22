#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "../common.h"

#include <QVariant>
#include <QString>
#include <QMap>

namespace Picto {

class Property
{
public:
	Property(int _type, QString _name, QVariant _value);

	int type();
	QString name();
	QVariant value();
	void setValue(QVariant _value);
	QMap<QString, QVariant> getAttributes();
	void addAttribute(QString _attributeName, QVariant _attributeValue);

private:
	int type_;
	QString name_;
	QVariant value_;
	QMap<QString, QVariant> attributes_;
};


}; //namespace Picto

#endif
