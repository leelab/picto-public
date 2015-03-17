#include "AlignSessionData.h"
#include <QMutexLocker>

/*! \brief Constructs an AlignSessionData object.
 *	@param dataType The type of data handled by this AlignSessionData object 
 *		(BEHAVIORAL_ALIGN_EVENTS_TYPE or NEURAL_ALIGN_EVENTS_TYPE from SessionInfo).
 *	@param matchedColumn The index of the value in each entry's QVariantList that
 *	indicates whether the current Alignment event has been matched yet by its counterpart
 *	from the other system to be aligned.
 */
AlignSessionData::AlignSessionData(int dataType, int matchedColumn)
:	dataType_(dataType),
	matchedCol_(matchedColumn)
{

}
AlignSessionData::~AlignSessionData()
{

}

/*! \brief Adds information about a single alignment entry to this object.
 *	@param dataId The dataId of this alignment data entry
 *	@param data The alignment data as a QVariantList where the "Matched" value must
 *	appear at the list index indicated in this object's constructor.
 */
void AlignSessionData::insertData(int dataId, QVariantList data)
{
	QVariantList dataList;
	dataList << dataId << data;	//Put dataId at the beginning of the list so that we can use it
								//in other functions.
	addData(dataType_,dataList);
}

/*! \brief Imlements SessionData::writeData() to write the input data to this object under the input
 *	dataType.
 *	\details Since no one ever copies or moves data to AlignSessionData objects, we know that any
 *	data coming in here is coming from insertData().  That means that the first entry is always 
 *	a DataId.  In this function, we make sure to write each Alignment event to this object's data
 *	map once and not write it again unless the Matched value is 1.  This way Alignment data that
 *	was resent by the Director, for example, can't overwrite an already matched Alignment entry.
 *	The DataId value is used to identify if a particular value has been written before.
 */
void AlignSessionData::writeData(int dataType, QVariantList data)
{
	Q_UNUSED(dataType);
	Q_ASSERT(dataType == dataType_);
	Q_ASSERT(data.size() > 0);
	int dataId = data[0].toInt();
	QVariantList originalList = data.mid(1,data.size()-1);
	if((data[matchedCol_].toInt() == 0) && dataMap_.contains(dataId))
		return;
	dataMap_[dataId] = originalList;
}

/*! \brief Implements SessionData::readDataTypes() to return only the single data type that
 *	was entered when this object was constructed.  Only it is allowed to be used with this object.
 */
QList<int>AlignSessionData::readDataTypes()
{
	return QList<int>() << dataType_;
}
/*! \brief Implements SessionData::readData() to read all data from this object.  When cut is true
 *	only alignment entries that have already been matched are removed.
 *	\note Only the dataType used to construct this object may be used.  If another type is used
 *	the returned list will be empty.
 */
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