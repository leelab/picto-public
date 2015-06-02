#ifndef _VIEW_PROPERTY_H_
#define _VIEW_PROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief A Property for storing ViewInformation.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ViewProperty : public Property
#else
class ViewProperty : public Property
#endif
{
	Q_OBJECT
public:
	ViewProperty(QString name, QVariant value);
	virtual ~ViewProperty(){};
	//! The ViewProperty's type is from the UserType region (UserType+2).
	static int typeId(){return QVariant::UserType+2;};

protected:
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;

	friend class PropertyContainer;
};


}; //namespace Picto

#endif
