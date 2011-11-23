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
	PropertyDataUnit(int index, QString value);

	void setActionFrame(qulonglong frameId){actionFrame_ = frameId;};
	qulonglong getActionFrame(){return actionFrame_;};
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	int index_;
	QString value_;
	qulonglong actionFrame_;

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


};


}; //namespace Picto

#endif
