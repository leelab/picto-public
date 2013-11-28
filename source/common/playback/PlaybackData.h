#ifndef _PLAYBACKDATA_H_
#define _PLAYBACKDATA_H_
#include "IndexedData.h"

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
template <class T>	// <type of index,type of stored data>
struct PlaybackData: public IndexedData
{
	PlaybackData(T data,double time,qulonglong pos = 0):IndexedData(PlaybackIndex(time,pos)){data_ = data;};
	virtual ~PlaybackData(){};
	T data_;
};
}; //namespace Picto
#endif