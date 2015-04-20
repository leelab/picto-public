#include "CachedSessionData.h"

CachedSessionData::CachedSessionData()
{

}
CachedSessionData::~CachedSessionData()
{

}

/*! \brief Adds a data entry of the input dataType with the input dataId to this object.
 *	\details Internally, this just adds dataId to the front of the data value and calls
 *	addData()
 */
void CachedSessionData::insertData(int dataType, int dataId, QVariantList data)
{
	QVariantList dataList;
	dataList << dataId << data;
	addData(dataType,dataList);
}

/*! \brief Implements SessionData::writeData() to write the input data entry to this objects 
 *	data structure for values of the input dataType.
 *	\details Since no one ever copies or moves data to CachedSessionData objects, we know that
 *	the value was written through insertData() and the first index in data will be the DataId.
 *	We use this to make sure that we never overwrite a value that was already written due to
 *	things like the Director resending data after a connection problem.
 */
void CachedSessionData::writeData(int dataType, QVariantList data)
{
	Q_ASSERT(data.size() > 0);
	int dataId = data[0].toInt();
	QVariantList originalList = data.mid(1,data.size()-1);
	if(dataMap_[dataType].contains(dataId))
		return;
	dataMap_[dataType][dataId] = originalList;
}

QList<int>CachedSessionData::readDataTypes()
{
	return dataMap_.keys();
}

/*! \brief Implements SessionData::readData() to return all data of the input DataType
 *	and delete it from this object if necessary.  
 *	\details The condition parameter is not used.
 */
QList<QVariantList> CachedSessionData::readData(int dataType,QVariant,bool cut)
{
	if(!dataMap_.contains(dataType))
		return QList<QVariantList>();
	QList<QVariantList> returnVal = dataMap_[dataType].values();
	if(cut)
	{
		dataMap_[dataType].clear();
		dataMap_.remove(dataType);
	}
	return returnVal;

}

void CachedSessionData::eraseEverything()
{
	dataMap_.clear();
}