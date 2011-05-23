#ifndef _POINTPROPERTY_H_
#define _POINTPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PointProperty : public Property
#else
class PointProperty : public Property
#endif
{
	Q_OBJECT
protected:
	PointProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	virtual void UpdateSerializationAttributesFromValue();
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	friend class PropertyContainer;
};


}; //namespace Picto

#endif