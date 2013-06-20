#ifndef _MapProperty_H_
#define _MapProperty_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a SizeProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API MapProperty : public Property
#else
class MapProperty : public Property
#endif
{
	Q_OBJECT
public:
	virtual ~MapProperty(){};
protected:
	MapProperty(QString name, QVariant value);
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif