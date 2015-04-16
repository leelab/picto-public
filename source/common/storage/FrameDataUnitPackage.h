#ifndef _FRAMEDATAUNITPACKAGE_H_
#define _FRAMEDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "FrameDataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores a list of FrameDataUnits, each of which holds a frame's first phosphor time, DataId, and the AssetId of the state that was active
 *	when the frame was rendered.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API FrameDataUnitPackage : public DataUnit
#else
class FrameDataUnitPackage : public DataUnit
#endif
{
public:

	FrameDataUnitPackage();

	void addFrame(double time, int stateId);
	void addFrame(QString time, int stateId);
	/*! \brief Removes all FrameDataUnits from this package.*/
	void clearFrames() { data_.clear(); };
	/*! \brief Returns the number of FrameDataUnits in this package.*/
	int length() { return data_.length(); }
	/*! \brief Returns the first FrameDataUnit in this package and removes it from the package.*/
	QSharedPointer<FrameDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	/*! \brief Returns the latest FrameDataUnit in this package and removes it from the package.*/
	QSharedPointer<FrameDataUnit> takeLastDataPoint() { return data_.takeLast(); };
	/*! \brief Returns the DataId of the latest frame in this package.*/
	qulonglong getLatestFrameId(){if(!data_.size())return -1;return data_.last()->getDataID();};

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<FrameDataUnit>> data_;
};


}; //namespace Picto

#endif
