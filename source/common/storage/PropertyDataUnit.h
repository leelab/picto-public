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
	//Creates a property data unit. If initValue is set,
	//the property unit will be marked as an init value.
	PropertyDataUnit(int index, bool initValue, QString value);

	void setActionFrame(qulonglong frameId){actionFrame_ = frameId;};
	qulonglong getActionFrame(){return actionFrame_;};
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	int index_;
	bool initValue_;
	QString value_;
	qulonglong actionFrame_;

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


};


}; //namespace Picto

#endif
