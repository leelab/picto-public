#ifndef _SESSIONDATAPACKAGE_H_
#define _SESSIONDATAPACKAGE_H_

#include <QMap>
#include "../common.h"
#include "DataUnit.h"
#include "TaskRunDataUnit.h"

namespace Picto {

/*!	\brief Unimplemented.
 *
 *	I'm not sure what this was going to be used for.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionDataPackage : public DataUnit
#else
class SessionDataPackage : public DataUnit
#endif
{
public:
	SessionDataPackage();

	void setTaskRun(QSharedPointer<TaskRunDataUnit> run);
	int getNumRuns(){return runsMap_.size();};
	QSharedPointer<TaskRunDataUnit> getTaskRunByIndex(int index);
	QSharedPointer<TaskRunDataUnit> getTaskRunByRunId(qulonglong id);
	QMap<qulonglong,QSharedPointer<TaskRunDataUnit>> getRunsMap();

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QMap<qulonglong,QSharedPointer<TaskRunDataUnit>> runsMap_;
	QList<QSharedPointer<TaskRunDataUnit>> runsList_;	//Built on demand for fast iterating.
};


}; //namespace Picto

#endif
