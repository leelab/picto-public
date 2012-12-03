#ifndef _VECTORDATASTATELIST_H_
#define _VECTORDATASTATELIST_H_
#include <QVector>
#include "DataStateList.h"

namespace Picto {
	
/*! \brief Holds Data State's Data
 */
	class VectorDataStateList : public DataStateList
{
public:
	VectorDataStateList();
	virtual ~VectorDataStateList();

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


private:
	int currCell();
	int moveToCell(int cell);
	//Returns the list index of the cell with the highest playback index less than or equal to
	//the input.
	int findIndexCell(PlaybackIndex index);

	//Recursively searches for the highest indexed cell with index <= input index
	//within the input window
	int indexSearch(PlaybackIndex index,int minCell, int maxCell);
	//Returns true if the input index is within the data limits of this VectorDataStateList

	int currentDataCell_;
	QList<QSharedPointer<IndexedData>> pbDataList_;
};


}; //namespace Picto
#endif