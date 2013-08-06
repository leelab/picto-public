#ifndef _ALIGN_SESSION_DATA_H_
#define _ALIGN_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>

#include "SessionData.h"
/*!	\brief Stores session data in RAM for quick access until it is saved to disk
 *
 */

class AlignSessionData : public SessionData
{
public:
	AlignSessionData::AlignSessionData(int dataType, int matchedColumn);
	virtual ~AlignSessionData();

	void insertData(int dataId, QVariantList data);

protected:
	//Should write the input data to descendant defined data structure
	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	virtual QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();
	QMap<int,QVariantList> dataMap_;
	int dataType_;
private:
	int matchedCol_;
};

#endif

