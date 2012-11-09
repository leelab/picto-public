#ifndef _PropertyTable_H_
#define _PropertyTable_H_
#include <QVector>
#include <QString>
#include <QSharedPointer>

#include "../common.h"
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
	void reportChangeInAllProperties();
	QVector<QSharedPointer<Property>> propTable_;
signals:
	void propertyChanged(QSharedPointer<Property> changedProp);
public slots:
	void updatePropertyValue(int index,QString value);
private:
	QSharedPointer<ExperimentConfig> expConfig_;
};


}; //namespace Picto

#endif
