#ifndef _ALIGNMENTINFO_H_
#define _ALIGNMENTINFO_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*! \brief Stores information about the relationship between a secondary timestream and the primary timestream in a Session.  
 *	
 *	\details In Picto, there is always a main timestream (From Director) and a secondary time stream (From Proxy).  This class creates
 *	objects representing the timing of the secondary time stream.  It stores Offset Time which is a constant difference between 
 *	the secondary timestream and the first (ie. It might have said 15 seconds when the primary said 0 seconds).  There is also a 
 *	temporalFactor which is the factor by which the primary time stream needs to be multipled in order to align it with the secondary time stream 
 *	(believe it or not this is sometimes an issue, particularly with the Pictobox Nidaq card, although we handle that differently since
 *	it doesn't accept event codes).  Essentially, by using Alignement Events, we calculate that \code 
		secondaryTimeStream = offsetTime + temporalFactor*primaryTimeStream
	\endcode
 *	and this class stores the constants needed for that equation.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	
	/*! \brief Returns the offset time.
	 *	\sa AlignmentInfo
	 */
	void setOffsetTime(double offsetTime){offsetTime_=offsetTime;};
	/*! \brief Sets the temporal factor.
	 *	\sa AlignmentInfo
	 */
	void setTemporalFactor(double temporalFactor){temporalFactor_=temporalFactor;};

	/*! \brief Returns the offset time.
	 *	\sa AlignmentInfo
	 */
	double getOffsetTime(){return offsetTime_;};
	/*! \brief Returns the temporal factor.
	 *	\sa AlignmentInfo
	 */
	double getTemporalFactor(){return temporalFactor_;};

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString sourceName_;
	double offsetTime_;
	double temporalFactor_;
};


}; //namespace Picto

#endif
