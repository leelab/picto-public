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
public:
	virtual ~ColorProperty(){};
protected:
	ColorProperty(QString name, QVariant value);
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif