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
	EnumProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	virtual QString valueString();
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
