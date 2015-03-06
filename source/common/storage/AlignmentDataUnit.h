#ifndef _ALIGNMENTDATAUNIT_H_
#define _ALIGNMENTDATAUNIT_H_
#include <QString>
#include <QList>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief A DataUnit used to send serialize/deserialize alignment data.
 *
 *	\details Alignment events are 7 bit codes sent from the Director to the Proxy to synchronize their timing.
 *	Both systems mark the time at which the alignment code occured, and the Directer, which generates the code
 *	also records the index of the event (Alignment Number, not limited to 7 bits).  They can then be matched up and used for
 *	a least squares method of time synchronization.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AlignmentDataUnit : public DataUnit
#else
class AlignmentDataUnit : public DataUnit
#endif
{
public:
	AlignmentDataUnit();
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setTimestamp(double timestamp){timestamp_ = timestamp;}
	/*! \brief Returns the alignment code (0->127).*/
	void setAlignCode(int alignCode){alignCode_ = alignCode;}
	/*! \brief Sets the alignment number.*/
	void setAlignNumber(int alignNumber){alignNumber_ = alignNumber;}

	/*! \brief Returns the time at which the alignment event occurred.*/
	double getTimestamp(){return timestamp_;}
	/*! \brief Returns the alignment code.*/
	int getAlignCode(){return alignCode_;}
	/*! \brief Returns true if this unit contains an alignment number (Proxy AlignmentDataUnits only have an AlignCode).*/
	int hasAlignNumber(){return (alignNumber_ != -1);};
	/*! \brief Returns the alignment number.*/
	int getAlignNumber(){return alignNumber_;}

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
double timestamp_;
int alignCode_;
int alignNumber_;
bool hasAlignNumber_;
};


}; //namespace Picto

#endif
