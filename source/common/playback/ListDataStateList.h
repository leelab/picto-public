#ifndef _LISTDATASTATELIST_H_
#define _LISTDATASTATELIST_H_
#include <QList>
#include <QLinkedList>
#include "DataStateList.h"

namespace Picto {

/*! \brief A lot has changed since the original Picto Playback Design, this class was part of an attempt
 *	to make code of different DataState types very reusable.  In the end, more code reusability led to
 *	less efficiency in terms of timing and memory space and we opted for a more tailored approach
 *	with each of the DataState classes doing a lot more of its own work to handle the DataState interface
 *	in the most efficient way possible.
 *
 *	We should probably remove this code.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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