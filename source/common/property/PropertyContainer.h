#ifndef _PROPERTYCONTAINER_H_
#define _PROPERTYCONTAINER_H_
#include <QtVariantProperty.h>
#include <QtPropertyBrowser.h>
#include <QString>
#include <QSharedPointer>

#include "EnumProperty.h"
#include "../common.h"

#include "Property.h"

namespace Picto {

class Property;
/*!	\brief A container that groups together a set of Property objects.
 *
 *	\details Since DataStore objects are required to be completely defined by their Property values, it is
 *	useful to have a container object which stores all of those Property objects and manages their creation,
 *	access and signaling.  This class defines that container object and allows DataStores to contain only a 
 *	single pointer to a PropertyContainer instead of maintaining lots of various lists filled with pointers to its Properties.
 *
 *	\note There is some history to this class.  It was once closely connected to the QtPropertyBrowser framework and some
 *	of the design decisions were made with that in mind.  Since then we have decoupled the QtPropertyBrowser
 *	framework in the interest of separating the design model and UI view; however, some design strangeness
 *	remains due to the original class design.  One issue that comes up frequently is the fact that at one
 *	point we allowed multiple Property objects in a PropertyContainer to have the same name.  We now require
 *	all Property objects from the same PropertyContainer to have different names, but there is still some left
 *	over code requiring inputting of an index along with a Property name to access a particular Property.  At
 *	some point we should remove this requirement.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyContainer : public QObject
#else
class PropertyContainer : public QObject
#endif
{
	Q_OBJECT

public:
	static QSharedPointer<PropertyContainer> create(QString _containerName);
	virtual ~PropertyContainer(){};
	void copyProperties(QSharedPointer<PropertyContainer> container2);
	static int enumTypeId();
	QSharedPointer<Property> addProperty(int _type, QString _identifier, QVariant _value);
	QVariant getPropertyValue(QString _identifier, int index=0);
	QString getPropertyName(QString _identifier, int index=0);
	QSharedPointer<Property> setPropertyValue(QString _identifier, QVariant _value, int index=0);
	void setPropertiesToInitValues();
	void setContainerName(QString _containerName);
	QString getContainerName();
	QStringList getPropertyList();
	/*! \brief Returns a lookup table with all of the contained Property objects indexed by their names.
	 *	\note At one point more than one Property with the same name was allowed.  This is not currently the case though
	 *	so although the returned Hash contains Vectors of Property objects, those Vectors should all have a size of one.
	 *	We should probably fix this at some point.
	 */
	QHash<QString, QVector<QSharedPointer<Property>>> getProperties(){return properties_;};
	QList<QSharedPointer<Property>> getRuntimeProperties();
	QSharedPointer<Property> getProperty(QString _identifier,int index=0);
	void setPropertiesAsAssociates(bool toAssociate);
	void clear();
signals:
	/*! \brief Emitted whenever the input Property's runValue changes.  The QVariant value is the value to which the Property's runValue changed.
	 *	\note This is emitted for all Properties contained in this PropertyContainer.
	 */
	void propertyValueChanged(Property* prop,QVariant);

private:
	PropertyContainer(QString _containerName);
	QHash<QString, QVector<QSharedPointer<Property>>> properties_;
	QString containerName_;
};


}; //namespace Picto

#endif
