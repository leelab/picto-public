#ifndef _BEHAVIORALDATAUNIT_H_
#define _BEHAVIORALDATAUNIT_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores behavioral a single unit of behavioral data as an x,y,t triplet
 *
 *	The behavioral unit data stores a single unit of behavioral data
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API BehavioralDataUnit : public DataUnit
#else
class BehavioralDataUnit : public DataUnit
#endif
{
public:
	BehavioralDataUnit();
	BehavioralDataUnit(double X, double Y, double T);
	BehavioralDataUnit(double X, double Y, QString T);
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	double x;
	double y;
	QString t;

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
