#ifndef _STATEDATAUNITPACKAGE_H_
#define _STATEDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "StateDataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores state data 
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API StateDataUnitPackage : public DataUnit
#else
class StateDataUnitPackage : public DataUnit
#endif
{
public:

	StateDataUnitPackage();

	void addTransition(QSharedPointer<Transition> transition);
	void addTransition(int id);
	void setActionFrame(qulonglong frameId);
	int length() { return data_.length(); }
	void clear() { data_.clear(); };
	QSharedPointer<StateDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	QSharedPointer<StateDataUnit> takeLastDataPoint() { return data_.takeLast(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<StateDataUnit>> data_;
};


}; //namespace Picto

#endif
