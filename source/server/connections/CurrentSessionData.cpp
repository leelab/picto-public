#include <QMutexLocker>
#include <QTime>
#include "CurrentSessionData.h"

/*! \brief Constructs a CurrentSessionData object for the input dataType.
 */
CurrentSessionData::CurrentSessionData(int dataType) :
dataType_(dataType)
{
	dataByTime_.clear();
}
CurrentSessionData::~CurrentSessionData()
{

}

/*! \brief Updates the value of the entry with the input variableId to the current input data if it
 *	doesn't yet have a value or if its value's dataId/timestamp is lower than the input dataId/timestamp.
 *	\details Internally, this adds dataId, variableId, and timestamp to the front of the data value, 
 *	creates a single member QList<QVariantList> with that VariableList and calls updateVariables()
 *	with it.
 */
void CurrentSessionData::updateVariable(int dataid,int variableId,QString timestamp,QString data)
{
	QVariantList dataList;
	dataList << dataid << variableId << timestamp << data; 
	updateVariables(QList<QVariantList>() << dataList);
}

/*! \brief Updates the stored values of all of the entries in the input data list 
 *	if the individual entries don't yet have values or their saved dataId/timestamps are lower than the
 *	dataId/timestamps included with these new values.
 *	\note The individual VariantList values in the QList must be structured as follows:
 *	int dataid,int variableId,QString timestamp,QString dataValue
 */
void CurrentSessionData::updateVariables(QList<QVariantList> data)
{
	//All data in the current session data has one type.  A variableId along with 
	//its serialized data, for this reason, we always use dataType of dataType_.
	addData(dataType_,data);
}

/*! \brief Implements SessionData::writeData() to write the input data to this object.
 *	\details The dataType parameter is ignored in this function, since all data types
 *	must be the same as the one entered into the constructor.  The data is assumed to
 *	be structured according to the documentation of updateVariables().
 *
 *	This function gets the DataId and timestamp from the input data.  If either one is
 *	greater than the stored DataId/Timestamp for the input data's variableId, the 
 *	new data replaces the old stored data for this variableId.  If that replacement
 *	occurs, the old stored value must also be removed from the dataByTime list
 *	which organizes value changes by their time order.  After the old data is removed
 *	from the dataByTime list, the new value is added to it with its timestamp as its
 *	key.
 */
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

/*! \brief Implements SessionData::readDataTypes() to return only the single data type that
 *	was entered when this object was constructed.  Only it is allowed to be used with this object.
 */
QList<int>CurrentSessionData::readDataTypes()
{
	return QList<int>() << dataType_;
}

/*! \brief Implements SessionData::readData() to return all data of the input dataType which
 *	has timestamp after that of the input condition.
 *	\details cut is implemented as expected, if true all returned values are deleted from this object.
 *	The input dataType must match the one that was used in this object's constructor.
 */
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