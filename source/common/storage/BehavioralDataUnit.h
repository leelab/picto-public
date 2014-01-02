#ifndef _BEHAVIORALDATAUNIT_H_
#define _BEHAVIORALDATAUNIT_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores a single unit (ie. the sample for a single instant in time) of x,y behavioral data
 *
 *	\details Behavioral data is just x,y SignalChannel data.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API BehavioralDataUnit : public DataUnit
#else
class BehavioralDataUnit : public DataUnit
#endif
{
public:
	BehavioralDataUnit();
	BehavioralDataUnit(double X, double Y);
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	double x;	//!< The X component of the data
	double y;	//!< The Y component of the data

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
