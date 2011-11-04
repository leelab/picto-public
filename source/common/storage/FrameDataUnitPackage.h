#ifndef _FRAMEDATAUNITPACKAGE_H_
#define _FRAMEDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "FrameDataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores frame data 
 *
 *	The behavioral data store is designed to information about the times 
 *	at which frames are rendered
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API FrameDataUnitPackage : public DataUnit
#else
class FrameDataUnitPackage : public DataUnit
#endif
{
public:

	FrameDataUnitPackage();

	void addFrame(int frameNumber, double time, QString stateName);
	void addFrame(int frameNumber, QString time, QString stateName);
	void clearFrames() { data_.clear(); };
	int length() { return data_.length(); }
	QSharedPointer<FrameDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	QSharedPointer<FrameDataUnit> takeLastDataPoint() { return data_.takeLast(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<FrameDataUnit>> data_;
};


}; //namespace Picto

#endif
