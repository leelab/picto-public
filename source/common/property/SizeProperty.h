#ifndef _SIZEPROPERTY_H_
#define _SIZEPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a SizeProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API SizeProperty : public Property
#else
class SizeProperty : public Property
#endif
{
	Q_OBJECT
public:
	virtual ~SizeProperty(){};
protected:
	SizeProperty(QString name, QVariant value);
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
