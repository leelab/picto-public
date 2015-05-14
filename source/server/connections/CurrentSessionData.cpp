#include <QMutexLocker>
#include <QTime>
#include <QDebug>

#include "CurrentSessionData.h"
#include "SessionInfoDefines.h"

/*! \brief Constructs a CurrentSessionData object for the input dataType.
 */
CurrentSessionData::CurrentSessionData(int dataType)
	: dataType_(dataType), capTime_(0)
{
	dataByTime_.clear();
	lastOffset_.clear();
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
	int dataBin = variableType;
	double timestamp = data[2].toDouble();
	int oldDataId = 0;
	double oldTimestamp = 0;
	bool bAppend = false;

	bool newCap = false;

	//For StateTransition Data, we use a Ringbuffer of size MAX_RETAIN_TRANSITION_INFO, but we need to append the data
	//	when we submit data of the same timestamp, or the data will be reordered across the boundary.
	if (variableType == TRANSITION_STATE_VAR_ID)
	{
		dataBin = variableType + lastOffset_[variableType];
		if (dataMap_.contains(dataBin) && dataMap_[dataBin][2].toDouble() == timestamp)
		{
			bAppend = true;
		}
		else
		{
			lastOffset_[variableType] = (lastOffset_[variableType] + 1) % MAX_RETAIN_TRANSITION_INFO;
			dataBin = variableType + lastOffset_[variableType];
			transitionsByTime_[timestamp] = dataBin;
		}

	}
	else if (variableType == FRAME_STATE_VAR_ID)
	{
		lastOffset_[variableType] = (lastOffset_[variableType] + 1) % MAX_RETAIN_FRAME_DATA;
		dataBin = variableType + lastOffset_[variableType];
	}
	else if (variableType >= 0) //Asset Property
	{
		//If variableType >= 0, then it's an Asset Property, so we need to use the MAX_RETAIN_PROPERTY_DATA
		dataBin = variableType + lastOffset_[variableType];
		if (dataMap_.contains(dataBin) && dataMap_[dataBin][2].toDouble() == timestamp)
		{
			bAppend = true;
		}
		else
		{
			lastOffset_[variableType] = (lastOffset_[variableType] + 1) % MAX_RETAIN_PROPERTY_DATA;
			dataBin = variableType + lastOffset_[variableType];

			int oldestRemainingBin = variableType + ((lastOffset_[variableType] + 1) % MAX_RETAIN_PROPERTY_DATA);
			if (dataMap_.contains(oldestRemainingBin))
			{
				if (dataMap_[oldestRemainingBin][2].toDouble() > capTime_)
				{
					capTime_ = dataMap_[oldestRemainingBin][2].toDouble();
					newCap = true;
				}
			}
		}
	}

	if (dataMap_.contains(dataBin))
	{
		oldDataId = dataMap_[dataBin][0].toInt();
		oldTimestamp = dataMap_[dataBin][2].toDouble();
	}

	if (oldDataId > dataId && !bAppend)
	{
		return;
	}

	if(oldTimestamp > timestamp)
		return;

	//If this has the same timestamp as the last chunk, we need to append the data to maintain the same ordering.
	//	If we don't, then the data will be reordered across the ring buffer's boundary
	if (bAppend)
	{
		dataMap_[dataBin][3] = dataMap_[dataBin][3].toString() + data[3].toString();
	}
	else
	{
		dataMap_[dataBin] = data;
	}

	if (!bAppend && dataByTime_.contains(oldTimestamp) && dataByTime_[oldTimestamp].contains(dataBin))
	{
		dataByTime_[oldTimestamp].remove(dataBin);
		if(dataByTime_[oldTimestamp].isEmpty())
			dataByTime_.remove(oldTimestamp);
	}
	dataByTime_[timestamp][dataBin] = variableType;

	//Cull all transitions, after the first, that are below the newCapTime
	if (newCap)
	{
		auto iter = transitionsByTime_.lowerBound(capTime_);

		//Go to the first bin Before the cutoff time.
		if (iter != transitionsByTime_.begin())
		{
			iter--;
		}
		qDebug() << "New Cap:" << capTime_;
		//Leave the first bin intact
		while (iter != transitionsByTime_.begin())
		{
			iter--;
			qDebug() << "Clearing out transition from:" << iter.key();
			dataMap_.remove(*iter);
			dataByTime_[iter.key()].remove(*iter);

			if (dataByTime_[iter.key()].isEmpty())
				dataByTime_.remove(iter.key());

			iter = transitionsByTime_.erase(iter);
		}
	}
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
	Q_UNUSED(dataType);
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

		auto transIter = transitionsByTime_.upperBound(condition.toDouble());
		for (QMap<double, int>::Iterator iter = transIter; iter != transitionsByTime_.end();)
		{
			iter = transitionsByTime_.erase(iter);
		}
	}
	return returnList;
}

void CurrentSessionData::eraseEverything()
{
	dataMap_.clear();
	dataByTime_.clear();
	lastOffset_.clear();
	transitionsByTime_.clear();
	capTime_ = 0.0;
}