#ifndef _LISTDATASTATELIST_H_
#define _LISTDATASTATELIST_H_
#include <QList>
#include <QLinkedList>
#include "DataStateList.h"

namespace Picto {
	
/*! \brief Holds Data State's Data
 */
	class ListDataStateList : public DataStateList<QList>
{
public:
	ListDataStateList();
	virtual ~ListDataStateList();
};

//Specialize parent class for QList type
template <>
QList<QSharedPointer<IndexedData>> DataStateList<QList>::getList(DataStateList<QList>::CellIter begin,DataStateList<QList>::CellIter end);

template <>
typename DataStateList<QList>::CellIter DataStateList<QList>::findIndexCell(PlaybackIndex index);


}; //namespace Picto
#endif