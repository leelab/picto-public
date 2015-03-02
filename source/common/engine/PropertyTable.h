#ifndef _PropertyTable_H_
#define _PropertyTable_H_
#include <QVector>
#include <QString>
#include <QSharedPointer>

#include "../common.h"
#include "../property/property.h"

#include "../storage/DesignConfig.h"

namespace Picto {

/*! \brief A collection of all of an Experiment's Properties used mainly for interfacing their values with the Picto Server.
 *	\details The PropertyTable was once a big hash table containing all Properties indexed by their AssetId.  At a certain point we realized that the 
 *	DesignConfig was the best place for handling that, and so the PropertyTable became essentially a fairly simple wrapper for a small portion of
 *	DesignConfig functionality.  It might better have been called PropertyUpdateInterface or something like that since it essentially allows us to take 
 *	a large group of unrelated Properties and simplify their interface so that all changed Property values can be condensed into a single list and a 
 *	list of Property initValues that need to be set to their respective properties can be set by simply calling a single function in a loop.
 *	
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyTable : public QObject
#else
class PropertyTable : public QObject
#endif
{
	Q_OBJECT
public:
	PropertyTable(QSharedPointer<DesignConfig> designConfig);
	void addProperty(QSharedPointer<Property> prop);
	void reportChangeInAllProperties();
signals:
	/*! \brief Emitted when a Property runValue changes.  changedProp is the Property whose runValue changed.*/
	void propertyValueChanged(Property* changedProp);
	/*! \brief Emitted when a Property's initValue changes.  changedProp is the Property whose initValue changed.*/
	void propertyInitValueChanged(Property* changedProp);
public slots:
	//If updateInitProps is false, values won't be updated for InitProperties
	void updateInitPropertyValue(int index,QString value);
private:
	QSharedPointer<DesignConfig> designConfig_;
private slots:
	void propValueChange(Property* changedProp,QVariant);
	void propInitValueChange(Property* changedProp,QVariant);
};


}; //namespace Picto

#endif
