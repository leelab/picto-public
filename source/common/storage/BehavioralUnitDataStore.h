#ifndef _BEHAVIORALUNITDATASTORE_H_
#define _BEHAVIORALUNITDATASTORE_H_

#include "../common.h"
#include "DataStore.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores behavioral a single unit of behavioral data as an x,y,t triplet
 *
 *	The behavioral unit data stores a single unit of behavioral data
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API BehavioralUnitDataStore : public DataStore
#else
class BehavioralUnitDataStore : public DataStore
#endif
{
public:
	BehavioralUnitDataStore();
	BehavioralUnitDataStore(double X, double Y, double T);
	
	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	double x;
	double y;
	double t;

};


}; //namespace Picto

#endif
