#ifndef _FRAMEDATAUNIT_H_
#define _FRAMEDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores a single frames rendering data 
 *
 *	Stores information about the times at which a frame was rendered
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API FrameDataUnit : public DataUnit
#else
class FrameDataUnit : public DataUnit
#endif
{
public:
	FrameDataUnit();
	FrameDataUnit(double timestamp, int stateId);
	FrameDataUnit(QString timestamp, int stateId);

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QString time;
	int stateId_;

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
