#include "CachedSessionData.h"
#include <QMutexLocker>

CachedSessionData::CachedSessionData()
{

}
CachedSessionData::~CachedSessionData()
{

}

void CachedSessionData::insertData(int dataType, int dataId, QVariantList data)
{
	QVariantList dataList;
	dataList << dataId << data;
	addData(dataType,dataList);
}

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