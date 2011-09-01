#ifndef _PROPERTYDATAUNIT_H_
#define _PROPERTYDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores a single unit of property data as a path, value, time triplet
 *
 *	The data in the PropertyDataUnit represents one property data transition.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API PropertyDataUnit : public DataUnit
#else
class PropertyDataUnit : public DataUnit
#endif
{
public:
	PropertyDataUnit();
	PropertyDataUnit(int index, QString path, QString value, double time);
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	int index_;
	QString path_;
	QString value_;
	double time_;

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
