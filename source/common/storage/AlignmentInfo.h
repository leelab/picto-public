#ifndef _ALIGNMENTINFO_H_
#define _ALIGNMENTINFO_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores Information about a signal channel
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API AlignmentInfo : public DataUnit
#else
class AlignmentInfo : public DataUnit
#endif
{
public:
	AlignmentInfo();
	AlignmentInfo(	double offsetTime,
					double temporalFactor);
	
	void setOffsetTime(double offsetTime){offsetTime_=offsetTime;};
	void setTemporalFactor(double temporalFactor){temporalFactor_=temporalFactor;};

	double getOffsetTime(){return offsetTime_;};
	double getTemporalFactor(){return temporalFactor_;};

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QString sourceName_;
	double offsetTime_;
	double temporalFactor_;
};


}; //namespace Picto

#endif
