#ifndef _INDEXEDDATA_H_
#define _INDEXEDDATA_H_
#include <QPair>

namespace Picto {
struct PlaybackIndex: public QPair<double,qulonglong>
{
	PlaybackIndex(){first = -1;second = 0;};
	PlaybackIndex(double time,qulonglong pos):QPair<double,qulonglong>(time,pos){};
	bool isValid(){return second > 0;};
	double& time(){return first;};
	qulonglong& pos(){return second;};
	PlaybackIndex nextIndexSameTime(){return PlaybackIndex(first,second+1);};
	PlaybackIndex prevIndexSameTime(){return PlaybackIndex(first,second-1);};
	static PlaybackIndex minForTime(double time){return PlaybackIndex(time,1);};
	static PlaybackIndex maxForTime(double time){return PlaybackIndex(time,0xFFFFFFFFFFFFFFFF);};
};

/*! \brief Component of Picto Playback system that stores data values from a session by index.
 */
struct IndexedData
{
	IndexedData(PlaybackIndex index){index_ = index;};
	virtual ~IndexedData(){};
	PlaybackIndex index_;
};
}; //namespace Picto
#endif