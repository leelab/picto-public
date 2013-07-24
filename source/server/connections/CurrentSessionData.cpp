#include "CurrentSessionData.h"
#include <QMutexLocker>

CurrentSessionData::CurrentSessionData()
{

}
CurrentSessionData::~CurrentSessionData()
{

}

void CurrentSessionData::updateVariable(int variableId,int dataid,QString timestamp,QString data)
{
	QVariantList dataList;
	dataList << variableId << dataid << timestamp << data;
	//All data in the current session data has one type.  A variableId along with 
	//its serialized data, for this reason, we always use dataType of zero. 
	addData(0,dataList);
}

void CurrentSessionData::writeData(int, QVariantList data)
{
	Q_ASSERT(data.size() == 4);
	int variableType = data[0].toInt();
	int dataId = data[1].toInt();
	double timestamp = data[2].toDouble();
	int oldDataId = dataMap_[variableType][1].toInt();
	double oldTimestamp = dataMap_[variableType][2].toDouble();
	if(oldDataId > dataId)
		return;
	if(oldTimestamp > timestamp)
		return;
	dataMap_[variableType] = data;
	dataByTime_.remove(oldTimestamp);
	dataByTime_[timestamp] = variableType;
}

QList<int>CurrentSessionData::readDataTypes()
{
	return QList<int>() << 0;
}

//In this function, condition is a timestamp after which all data is read
QList<QVariantList> CurrentSessionData::readData(int,QVariant condition,bool cut)
{
	//Get an iterator to the first
	QMap<double,int>::Iterator startIter = dataByTime_.upperBound(condition.toDouble());
	QList<QVariantList> returnList;
	for(QMap<double,int>::Iterator iter = startIter;iter != dataByTime_.end();iter++)
	{
		Q_ASSERT(dataMap_.contains(iter.value()));
		returnList.append(dataMap_.value(iter.value()));
	}
	if(cut)
	{
		for(QMap<double,int>::Iterator iter = startIter;iter != dataByTime_.end();iter++)
		{
			dataMap_.remove(iter.value());
			dataByTime_.erase(iter);
		}
	}
	return returnList;
}