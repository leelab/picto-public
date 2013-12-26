#ifndef _ListProperty_H_
#define _ListProperty_H_

#include "Property.h"

namespace Picto {
/*!	\brief A Property for storing List data.
 *	This is used for storing lists of strings.  A number of DataStore objects need to store variable sized
 *	lists.  For example, a TokenFactoryGraphic needs to store lists of taken colors and positions.  This is 
 *	the Property that they use to do that.
 *	\note Typically, it doesn't make sense to make a ListProperty editable by the designer or Experiment operator.  ListProperties
 *	are usually set invisible to the GUI (setVisible()) and used exclusively as a way to describe an DataStore's current state
 *	to support the requirement that that state be fully described by the DataStore's Properties.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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