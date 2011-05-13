#ifndef _FRAME_UNIT_DATA_STORE_H_
#define _FRAME_UNIT_DATA_STORE_H_

#include "../common.h"
#include "DataStore.h"

#include <QString>

namespace Picto {

/*!	\brief Stores a single frames rendering data 
 *
 *	Stores information about the times at which a frame was rendered
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API FrameUnitDataStore : public DataStore
#else
class FrameUnitDataStore : public DataStore
#endif
{
	Q_OBJECT
public:
	FrameUnitDataStore();
	FrameUnitDataStore(int frameNum, double timestamp, QString statename);

	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	int frameNumber; 
	double time;
	QString stateName;
};


}; //namespace Picto

#endif
