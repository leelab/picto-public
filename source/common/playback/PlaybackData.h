#ifndef _PLAYBACKDATA_H_
#define _PLAYBACKDATA_H_
#include "IndexedData.h"

namespace Picto {
/*! \brief Component of Picto Playback system that stores data values from a session.
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