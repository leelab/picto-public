#ifndef _LINKEDDATASTATELIST_H_
#define _LINKEDDATASTATELIST_H_
#include <QLinkedList>
#include "DataStateList.h"

namespace Picto {

typedef QLinkedList<QSharedPointer<IndexedData>> ListType;
typedef ListType::iterator CellIter;
/*! \brief Holds Data State's Data.
 */
	class DataStateList : public DataStateList
{
public:
	DataStateList();
	virtual ~DataStateList();

	void clear();
	int size();
	void append(QSharedPointer<IndexedData> data);
	void clearBefore(double time);

	QSharedPointer<IndexedData> currValue();
	QSharedPointer<IndexedData> nextValue();
	QSharedPointer<IndexedData> next2Value();
	QSharedPointer<IndexedData> firstValue();
	QSharedPointer<IndexedData> lastValue();

	PlaybackIndex currIndex();
	PlaybackIndex nextIndex();
	PlaybackIndex next2Index();
	PlaybackIndex firstIndex();
	PlaybackIndex lastIndex();

	PlaybackIndex moveToCell(PlaybackIndex index);
	PlaybackIndex moveToNext();

	QList<QSharedPointer<IndexedData>> getValuesSince(double time);
	QList<QSharedPointer<IndexedData>> getValuesUntil(double time);


protected:
	//Returns a list of Indexed Data pointers starting at the begin iterator and ending
	//at the end iterator.
	virtual QList<QSharedPointer<IndexedData>> getList(CellIter begin,CellIter end);
	//Returns the list index of the cell with the highest playback index less than or equal to
	//the input.
	virtual CellIter findIndexCell(PlaybackIndex index);

private:

	ListType::iterator currentDataCell_;
	ListType pbDataList_;
	bool initialized_;
};


}; //namespace Picto
#endif