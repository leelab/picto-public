#ifndef _ListProperty_H_
#define _ListProperty_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a SizeProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ListProperty : public Property
#else
class ListProperty : public Property
#endif
{
	Q_OBJECT
public:
	virtual ~ListProperty(){};
protected:
	ListProperty(QString name, QVariant value);
	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	friend class PropertyContainer;
};


}; //namespace Picto

#endif