#include <QMutexLocker>
#include <QTime>
#include "CurrentSessionData.h"

CurrentSessionData::CurrentSessionData(int dataType) :
dataType_(dataType)
{
	dataByTime_.clear();
}
CurrentSessionData::~CurrentSessionData()
{

}

void CurrentSessionData::updateVariable(int dataid,int variableId,QString timestamp,QString data)
{
	QVariantList dataList;
	dataList << dataid << variableId << timestamp << data; 
	updateVariables(QList<QVariantList>() << dataList);
}

void CurrentSessionData::updateVariables(QList<QVariantList> data)
{
	//All data in the current session data has one type.  A variableId along with 
	//its serialized data, for this reason, we always use dataType of dataType_.
	addData(dataType_,data);
}

void CurrentSessionData::writeData(int, QVariantList data)
{
	Q_ASSERT(data.size() == 4);
	int dataId = data[0].toInt();
	int variableType = data[1].toInt();
	double timestamp = data[2].toDouble();
	int oldDataId = 0;
	double oldTimestamp = 0;
	if(dataMap_.contains(variableType))
	{
		oldDataId = dataMap_[variableType][0].toInt();
		oldTimestamp = dataMap_[variableType][2].toDouble();
	}
	if(oldDataId > dataId)
		return;
	if(oldTimestamp > timestamp)
		return;
	//qDebug(data[3].toString().toLatin1() + "\n");
	dataMap_[variableType] = data;
	if(dataByTime_.contains(oldTimestamp) && dataByTime_[oldTimestamp].contains(variableType))
	{
		dataByTime_[oldTimestamp].remove(variableType);
		if(dataByTime_[oldTimestamp].isEmpty())
			dataByTime_.remove(oldTimestamp);
	}
	dataByTime_[timestamp][variableType] = variableType;
}

QList<int>CurrentSessionData::readDataTypes()
{
	return QList<int>() << dataType_;
}

//In this function, condition is a timestamp after which all data is read
QList<QVariantList> CurrentSessionData::readData(int dataType,QVariant condition,bool cut)
{
	Q_ASSERT(dataType == dataType_);
	//Get an iterator to the first
	QMap<double,QMap<int,int>>::Iterator startIter = dataByTime_.upperBound(condition.toDouble());
	QList<QVariantList> returnList;
	for(QMap<double,QMap<int,int>>::Iterator iter = startIter;iter != dataByTime_.end();iter++)
	{
		foreach(int varType,iter.value().keys())
		{
			Q_ASSERT(dataMap_.contains(varType));
			returnList.append(dataMap_.value(varType));
		}
	}
	if(cut)
	{
		for(QMap<double,QMap<int,int>>::Iterator iter = startIter;iter != dataByTime_.end();)
		{
			foreach(int varType,iter.value().keys())
			{
				dataMap_.remove(varType);
			}
			iter = dataByTime_.erase(iter);
		}
	}
	return returnList;
}

void CurrentSessionData::eraseEverything()
{
	dataMap_.clear();
}