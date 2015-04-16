#ifndef _INPUTDATAUNIT_H_
#define _INPUTDATAUNIT_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores a single unit (ie. the sample for a single instant in time) of Input Signal data
 *
 *	\details Input data is just 8 channel AnalogSignal data.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API InputDataUnit : public DataUnit
#else
	class InputDataUnit : public DataUnit
#endif
{
public:
	InputDataUnit();
	InputDataUnit(double Input[8]);
	virtual ~InputDataUnit() {};
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	double input[8];	//!< The 8 input channels

};


}; //namespace Picto

#endif
