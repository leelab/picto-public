#include <QMutexLocker>
#include <QThread>
#include "ListDataStateList.h"
using namespace Picto;			

//Template function definitions must be in header file-------------------------------------------
ListDataStateList::ListDataStateList()
{
}

ListDataStateList::~ListDataStateList()
{

}


//Specialize parent class for QList type
template <>
QList<QSharedPointer<IndexedData>> DataStateList<QList>::getList(DataStateList<QList>::CellIter begin,DataStateList<QList>::CellIter end)
{
	int pos = begin - pbDataList_.begin();
	int length = end-begin;
	if(end == pbDataList_.end())
		return pbDataList_.mid(pos);
	return pbDataList_.mid(pos,length);
}



QList<QSharedPointer<IndexedData>>::iterator cellSearch(PlaybackIndex index,QList<QSharedPointer<IndexedData>>::iterator minCell,QList<QSharedPointer<IndexedData>>::iterator maxCell,QList<QSharedPointer<IndexedData>>::iterator errorCell)
{
	//Stop condition
	if(maxCell == minCell)
	{
		PlaybackIndex sIndex = (*minCell)->index_;
		if(sIndex <= index)
		{
			return minCell;
		}
		return errorCell;
	}
	//Reduce search space
	QList<QSharedPointer<IndexedData>>::iterator midCell = minCell+1+(maxCell-minCell)/2;
	if((*midCell)->index_ <= index)
		return cellSearch(index,midCell,maxCell,errorCell);
	return cellSearch(index,minCell,midCell-1,errorCell);
}





template <>
typename DataStateList<QList>::CellIter DataStateList<QList>::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return pbDataList_.end();
	if(!size())
		return pbDataList_.end();
	return cellSearch(index,pbDataList_.begin(),pbDataList_.end()-1,pbDataList_.end());
}
