#ifndef _SESSIONDATAPACKAGE_H_
#define _SESSIONDATAPACKAGE_H_

#include <QMap>
#include "../common.h"
#include "DataUnit.h"
#include "TaskRunDataUnit.h"

namespace Picto {

/*!	\brief Stores information about all Task runs in a given session.
 *
 *	\details A Session can be viewed as a collection of Task runs (we often refer to these simply as "runs").
 *	Objects of this class store a collection of TaskRunDataUnit objects containing meta-data about all of
 *	the runs that made up a particular session.  Runs can be accessed by their DataId values or their
 *	order in the session.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	/*! \brief Returns the number of Runs stored in this SessionDataPackage.*/
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
