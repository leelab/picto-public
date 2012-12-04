#ifndef _LINKEDDATASTATELIST_H_
#define _LINKEDDATASTATELIST_H_
#include <QLinkedList>
#include "DataStateList.h"

namespace Picto {

/*! \brief Holds Data State's Data
 */
	class LinkedDataStateList : public DataStateList<QLinkedList>
{
public:
	LinkedDataStateList();
	virtual ~LinkedDataStateList();
};

template <>
QList<QSharedPointer<IndexedData>> DataStateList<QLinkedList>::getList(DataStateList<QLinkedList>::CellIter begin,DataStateList<QLinkedList>::CellIter end);

template <>
typename DataStateList<QLinkedList>::CellIter DataStateList<QLinkedList>::findIndexCell(PlaybackIndex index);


}; //namespace Picto
#endif