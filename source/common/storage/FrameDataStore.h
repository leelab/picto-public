#ifndef _FRAME_DATA_STORE_H_
#define _FRAME_DATA_STORE_H_

#include "../common.h"
#include "DataStore.h"
#include "FrameUnitDataStore.h"

#include <QString>

namespace Picto {

/*!	\brief Stores frame data 
 *
 *	The behavioral data store is designed to information about the times 
 *	at which frames are rendered
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API FrameDataStore : public DataStore
#else
class FrameDataStore : public DataStore
#endif
{
public:

	FrameDataStore();

	void addFrame(int frameNumber, double time, QString stateName);
	void clearFrames() { data_.clear(); };
	int length() { return data_.length(); }
	FrameUnitDataStore takeFirstDataPoint() { return data_.takeFirst(); };
	FrameUnitDataStore takeLastDataPoint() { return data_.takeLast(); };

	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:

	QList<FrameUnitDataStore> data_;
};


}; //namespace Picto

#endif
