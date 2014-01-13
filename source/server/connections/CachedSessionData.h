#ifndef _CACHED_SESSION_DATA_H_
#define _CACHED_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>

#include "SessionData.h"
/*!	\brief Stores session data in RAM for quick access until it is saved to disk.
 *	\details Data is stored in a lookup table of lookup tables, where the outer table
 *	is indexed by dataType, and the inner table is indexed by the DataId of each particular
 *	event.  By using this double lookup table we keep each data type separate and make
 *	sure that each individual data value is only ever written once even if it enters
 *	the writeData() function more than once.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	QList<int> readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	virtual QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();
	QMap<int,QMap<int,QVariantList>> dataMap_;

};

#endif

