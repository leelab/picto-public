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
public:
	virtual ~RectProperty(){};
protected:
	RectProperty(QString name, QVariant value);
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	virtual QVariant attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
