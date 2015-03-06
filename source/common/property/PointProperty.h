#ifndef _POINTPROPERTY_H_
#define _POINTPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief A Property for storing x,y position data.
 *	This is useful for storing the x,y positions of VisualElements.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PointProperty : public Property
#else
class PointProperty : public Property
#endif
{
	Q_OBJECT
public:
	virtual ~PointProperty(){};
protected:
	PointProperty(QString name, QVariant value);
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	virtual QVariant attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif