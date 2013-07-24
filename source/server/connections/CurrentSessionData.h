#ifndef _CURRENT_SESSION_DATA_H_
#define _CURRENT_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QPair>

#include "SessionData.h"

/*!	\brief Interface to an object that stores session data
 *
 */

class CurrentSessionData : public SessionData
{
public:
	CurrentSessionData::CurrentSessionData();
	virtual ~CurrentSessionData();

	void updateVariable(int variableId,int dataid,QString timestamp,QString data);

protected:

	//Should write the input data to descendant defined data structure
	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
private:
	QMap<int,QVariantList> dataMap_;
	QMap<double,int> dataByTime_;
};

#endif

