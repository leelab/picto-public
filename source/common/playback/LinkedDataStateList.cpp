#include <QMutexLocker>
#include <QThread>
#include "LinkedDataStateList.h"
using namespace Picto;			

//Template function definitions must be in header file-------------------------------------------
LinkedDataStateList::LinkedDataStateList()
{
}

LinkedDataStateList::~LinkedDataStateList()
{

}

//Specialization of Parent class for QLinkedList container type
template <>
QList<QSharedPointer<IndexedData>> DataStateList<QLinkedList>::getList(DataStateList<QLinkedList>::CellIter begin,DataStateList<QLinkedList>::CellIter end)
{
	QList<QSharedPointer<IndexedData>> returnVal;
	for(CellIter iter = begin;iter!=end;iter++)
	{
		returnVal.append(*iter);
	}
	return returnVal;
}

template <>
typename DataStateList<QLinkedList>::CellIter DataStateList<QLinkedList>::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return pbDataList_.end();
	if(!size())
		return pbDataList_.end();
	CellIter finder = currentDataCell_;
	if(finder == pbDataList_.end())
		finder = pbDataList_.begin();
	int inc = 1;
	if((*finder)->index_ > index)
		inc = -1;
	while	(	
				(inc < 0 || finder+1 != pbDataList_.end())
				&& (inc > 0 || finder != pbDataList_.begin()) 
				&& !(	
						(	(*finder)->index_ <= index	) 
						&&	((*(finder+1))->index_ > index	)
					)
			)
	{
		finder = finder+inc;
	}
	if(finder == pbDataList_.begin() && (*finder)->index_ > index)
	{
		finder = pbDataList_.end();
	}
	return finder;
}