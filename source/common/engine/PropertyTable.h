#ifndef _PropertyTable_H_
#define _PropertyTable_H_

#include "../common.h"
#include <QVector>
#include <QString>
#include <QSharedPointer>
#include "../property/property.h"
#include "../storage/propertylookup.h"
#include "../storage/ExperimentConfig.h"

namespace Picto {

/*! PropertyTable
 *
 *	Maps Properties to indeces.  These speeds up property data transfer by allowing
 *	property indexing by number rather than hierarchichal path.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyTable : public QObject
#else
class PropertyTable : public QObject
#endif
{
	Q_OBJECT
public:
	PropertyTable(QSharedPointer<ExperimentConfig> expConfig);
	void clear(){propTable_.clear();};
	void addProperty(QSharedPointer<Property> prop);
	void updatePropertyValue(int index,QString value);
	void reportChangeInAllProperties();
	QVector<QSharedPointer<Property>> propTable_;
	//QSharedPointer<PropertyLookup> getPropLookup();
signals:
	void propertyChanged(QSharedPointer<Property> changedProp);
private:
	QSharedPointer<ExperimentConfig> expConfig_;
	//QSharedPointer<PropertyLookup> propLookup_;
};


}; //namespace Picto

#endif
