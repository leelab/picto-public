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
	EnumProperty(QString name, QVariant value);
	virtual ~EnumProperty(){};
	static int typeId(){return QVariant::UserType+1;};
	
protected:
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
