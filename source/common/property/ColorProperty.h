#ifndef _COLORPROPERTY_H_
#define _COLORPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ColorProperty : public Property
#else
class ColorProperty : public Property
#endif
{
	Q_OBJECT
protected:
	ColorProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	virtual void UpdateSerializationAttributesFromValue();
	virtual void SetValueFromDeserializedData(QVariant _value);
private:
	friend class PropertyContainer;
};


}; //namespace Picto

#endif