#ifndef _VECTORDATASTATELIST_H_
#define _VECTORDATASTATELIST_H_
#include <QVector>
#include "DataStateList.h"

namespace Picto {
	
/*! \brief Holds Data State's Data
 */
	class VectorDataStateList : public DataStateList<QVector>
{
public:
	VectorDataStateList();
	virtual ~VectorDataStateList();
};

//Specialize parent class for QList type
template <>
QList<QSharedPointer<IndexedData>> DataStateList<QVector>::getList(DataStateList<QVector>::CellIter begin,DataStateList<QVector>::CellIter end);

template <>
typename DataStateList<QVector>::CellIter DataStateList<QVector>::findIndexCell(PlaybackIndex index);


}; //namespace Picto
#endif