#ifndef _LINKEDDATASTATELIST_H_
#define _LINKEDDATASTATELIST_H_
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