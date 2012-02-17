#ifndef _ENUMPROPERTY_H_
#define _ENUMPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API EnumProperty : public Property
#else
class EnumProperty : public Property
#endif
{
	Q_OBJECT
public:
	EnumProperty(QtVariantProperty* variantProp, QtVariantPropertyManager* manager);
	virtual ~EnumProperty(){};
	
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual QString valueString();
	QString toUserString();
	void fromUserString(QString userString);
private:
	void set(QVariant _value) { SetValueFromString(_value,QSharedPointer<QXmlStreamReader>()); };
	QVariant get(){return (QVariant)valueString();};
	QVariant getIndex(){return (QVariant)value().toInt();};
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
