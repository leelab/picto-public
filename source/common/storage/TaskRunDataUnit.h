#ifndef _TASKRUNDATAUNIT_H_
#define _TASKRUNDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores information about a single run of a task (frequently referred to as simply a "run").
 *
 *	\details Objects of this class store the start and end frames of a run along with various data
 *	set by the operator during the run.  This data includes the run's name (which the operator may
 *	or may not have changed from its default value), notes about the run that the operator added, 
 *	and a boolean flag indicating whether the operator considered this run to be worth saving.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API TaskRunDataUnit : public DataUnit
#else
class TaskRunDataUnit : public DataUnit
#endif
{
public:
	TaskRunDataUnit();
	TaskRunDataUnit(qulonglong startFrame, QString name, QString notes, bool saved=false);
	TaskRunDataUnit(qulonglong startFrame, qulonglong endFrame, QString name, QString notes, bool saved=false);

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	qulonglong startFrame_;	//!< Stores the DataId of the first frame in this run.
	qulonglong endFrame_;	//!< Stores the DataId of the last frame in this run.
	QString name_;	//!< Stores the name that the operator set for this run.
	QString notes_;	//!< Stores the notes that the operator saved including information about this run.
	bool saved_;	//!< Stores whether the operator marked this run as saved.
};


}; //namespace Picto

#endif
