#ifndef _VECTORDATASTATELIST_H_
#define _VECTORDATASTATELIST_H_
#include <QVector>
#include "DataStateList.h"

namespace Picto {
	
/*!	\brief A parameter for containing numeric integer values.
 *
 *	We wanted to get rid of this parameter, because it is really just a RangeParameter with less options.  Since old 
 *	Experiments use this Parameter though, the easiest way to support them in newer code without allowing this Parameter
 *	into new designs was to keep it in Picto, but now let it show up in the Designer Toolbox, so that is what we have done.
 *	This Parameter stores integer values.  It does the same thing that the RangeParameter
 *	does except without the set range (ie. The range is just from INT_MIN to INT_MAX).
 *	The Value Property is runtime editable and appears as a numeric "integer only" input widget
 *	in the PropertyFrame.  The numeric value is accessible through the javascript "value" property as:
 *	\code
 		NumericParameterName.value = 0.26;
 		var numVal = NumericParameterName.value;
 	\endcode
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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