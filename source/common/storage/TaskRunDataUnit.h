#ifndef _TASKRUNDATAUNIT_H_
#define _TASKRUNDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores a single frames rendering data 
 *
 *	Stores information about the times at which a frame was rendered
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

	qulonglong startFrame_;
	qulonglong endFrame_;
	QString name_;
	QString notes_;
	bool saved_;

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
