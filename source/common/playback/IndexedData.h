#ifndef _INDEXEDDATA_H_
#define _INDEXEDDATA_H_

namespace Picto {
/*! \brief Component of Picto Playback system that stores data values from a session by index.
 */
template <class I>	// <type of index>
struct IndexedData
{
	IndexedData(double time,I index){time_ = time;index_ = index;};
	virtual ~IndexedData(){};
	double time_;
	I index_;
};
}; //namespace Picto
#endif