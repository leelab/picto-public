#ifndef _CACHED_SESSION_DATA_H_
#define _CACHED_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>

#include "SessionData.h"
/*!	\brief Stores session data in RAM for quick access until it is saved to disk
 *
 */

class CachedSessionData : public SessionData
{
public:
	CachedSessionData::CachedSessionData();
	virtual ~CachedSessionData();

	void insertData(int dataType, int dataId, QVariantList data);
protected:

	//Should write the input data to descendant defined data structure
	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);

private:
	QMap<int,QMap<int,QVariantList>> dataMap_;
};

#endif

