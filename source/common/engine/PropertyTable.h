#ifndef _PropertyTable_H_
#define _PropertyTable_H_

#include "../common.h"
#include <QVector>
#include <QString>
#include <QSharedPointer>
#include "../property/property.h"

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
	PropertyTable();
	void clear(){propTable_.clear();};
	void addProperty(QSharedPointer<Property> prop);
	void updatePropertyValue(int index,QString value);
	void reportChangeInAllProperties();
signals:
	void propertyChanged(QSharedPointer<Property> changedProp);
private:
	QVector<QSharedPointer<Property>> propTable_;
};


}; //namespace Picto

#endif
