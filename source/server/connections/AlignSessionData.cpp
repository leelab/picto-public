#include "AlignSessionData.h"
#include <QMutexLocker>

AlignSessionData::AlignSessionData(int dataType, int matchedColumn)
:	dataType_(dataType),
	matchedCol_(matchedColumn)
{

}
AlignSessionData::~AlignSessionData()
{

}

void AlignSessionData::insertData(int dataId, QVariantList data)
{
	QVariantList dataList;
	dataList << dataId << data;
	addData(dataType_,dataList);
}

void AlignSessionData::writeData(int dataType, QVariantList data)
{
	Q_ASSERT(dataType == dataType_);
	Q_ASSERT(data.size() > 0);
	int dataId = data[0].toInt();
	QVariantList originalList = data.mid(1,data.size()-1);
	if((data[matchedCol_].toInt() == 0) && dataMap_.contains(dataId))
		return;
	dataMap_[dataId] = originalList;
}

QList<int>AlignSessionData::readDataTypes()
{
	return QList<int>() << dataType_;
}

QList<QVariantList> AlignSessionData::readData(int dataType,QVariant,bool cut)
{
	if(!dataType == dataType_)
		return QList<QVariantList>();
	QList<QVariantList> returnVal = dataMap_.values();
	if(returnVal.isEmpty())
		return returnVal;
	//For AlignSessionData, we don't cut any values that are unmatched and still matchable.
	//ie. matched == 0
	if(cut)
	{
		QMap<int,QVariantList> dataMapCopy = dataMap_;
		dataMap_.clear();
		for(QMap<int,QVariantList>::Iterator it = dataMapCopy.begin(); it!=dataMapCopy.end();it++)
		{
			if(it.value()[matchedCol_].toInt() == 0)
				dataMap_[it.key()] = it.value();
		}
	}
	return returnVal;
}

void AlignSessionData::eraseEverything()
{
	dataMap_.clear();
}