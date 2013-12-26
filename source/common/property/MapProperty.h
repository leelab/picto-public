#ifndef _MapProperty_H_
#define _MapProperty_H_

#include "Property.h"

namespace Picto {
/*!	\brief A Property for storing Map (lookup table) data.
 *	This is used for storing a map of key, value pairs.  A number of DataStore objects need to store key value pair structures.  
 *	A VariantMap. for example, needs to do this by its very definition.  This is 
 *	the Property that is used for that purpose.
 *	\note Typically, it doesn't make sense to make a MapProperty editable by the designer or Experiment operator.  MapProperty
 *	are usually set invisible to the GUI (setVisible()) and used exclusively as a way to describe an DataStore's current state
 *	to support the requirement that that state be fully described by the DataStore's Properties.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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