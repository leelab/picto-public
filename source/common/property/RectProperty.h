#ifndef _RECTPROPERTY_H_
#define _RECTPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API RectProperty : public Property
#else
class RectProperty : public Property
#endif
{
	Q_OBJECT
protected:
	RectProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	virtual void UpdateSerializationAttributesFromValue();
	virtual void SetValueFromDeserializedData(QVariant _value);
private:
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
