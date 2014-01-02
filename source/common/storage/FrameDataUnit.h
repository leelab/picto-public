#ifndef _FRAMEDATAUNIT_H_
#define _FRAMEDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores information about rendering of a single frame.
 *
 *	\details Stored data is the first phosphor time and the AssetId of the State that was active
 *	when the frame was rendered.
 *	\note In this and some other DataUnits, we store a timestamp as a string to be sure that no one
 *	reduces the precision of the Timestamp anywhere in transit (as might happen if some other class
 *	converted a timestamp double to a string with unknown precision).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	QString time;	//!< The time of the frame's first phosphor.
	int stateId_;	//!< The AssetId of the State that was active when this frame was displayed.

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
