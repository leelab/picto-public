#ifndef _SIZEPROPERTY_H_
#define _SIZEPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief A Property for storing width,height size data.
 *	This is useful for storing the size of VisualElements.  It is almost exactly the same as the PointProperty
 *	class with the exception that it deals with QVariant(QSize) values instead of QVariant(QPoint).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
