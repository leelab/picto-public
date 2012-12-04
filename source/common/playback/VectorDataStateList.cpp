#include <QMutexLocker>
#include <QThread>
#include "VectorDataStateList.h"
using namespace Picto;			

//Template function definitions must be in header file-------------------------------------------
VectorDataStateList::VectorDataStateList()
{
}

VectorDataStateList::~VectorDataStateList()
{

}


//Specialize parent class for QList type
template <>
QList<QSharedPointer<IndexedData>> DataStateList<QVector>::getList(DataStateList<QVector>::CellIter begin,DataStateList<QVector>::CellIter end)
{
	int pos = begin - pbDataList_.begin();
	int length = end-begin;
	if(end == pbDataList_.end())
		return pbDataList_.mid(pos).toList();
	return pbDataList_.mid(pos,length).toList();
}



QVector<QSharedPointer<IndexedData>>::iterator cellSearch(PlaybackIndex index,QVector<QSharedPointer<IndexedData>>::iterator minCell,QVector<QSharedPointer<IndexedData>>::iterator maxCell,QVector<QSharedPointer<IndexedData>>::iterator errorCell)
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
	QVector<QSharedPointer<IndexedData>>::iterator midCell = minCell+1+(maxCell-minCell)/2;
	if((*midCell)->index_ <= index)
		return cellSearch(index,midCell,maxCell,errorCell);
	return cellSearch(index,minCell,midCell-1,errorCell);
}





template <>
typename DataStateList<QVector>::CellIter DataStateList<QVector>::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return pbDataList_.end();
	if(!size())
		return pbDataList_.end();
	return cellSearch(index,pbDataList_.begin(),pbDataList_.end()-1,pbDataList_.end());
}
