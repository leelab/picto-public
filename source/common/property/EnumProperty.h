#ifndef _ENUMPROPERTY_H_
#define _ENUMPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief A Property for storing enums.
 *	\details This is useful for storing a list of options that the designer can select.  The list of possible
 *	enum values is set by adding a Property Attribute with the name "enumNames" and a QVariant of 
 *	a QStringList containing enum names for the value.  This is done using Property::setAttribute().
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Since there is no QVariant::Enum type, the EnumProperty's type is 
	 *	from the UserType region (UserType+1).
	 */
	static int typeId(){return QVariant::UserType+1;};
	
protected:
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
