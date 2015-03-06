#ifndef _LiveRunNotesReader_H_
#define _LiveRunNotesReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the RunNotesReader class for a live test experiment.
 *	\details Since run notes are pretty meaningless in a test experiment, but we want Analysis elements
 *	that poll run notes data to return something, this class pretty much just extends the RunNotesReader
 *	functions to return simple fake data like the "Sample Name" name and the "Sample Notes" notes.  Start
 *	time is zero and end time is always -1 (since we don't know how long the run is while we're still in it).
 *	Run index is zero.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveRunNotesReader : public RunNotesReader
#else
class LiveRunNotesReader : public RunNotesReader
#endif
{
	Q_OBJECT
public:
	LiveRunNotesReader(){};

	//Run Notes Reader Interface
	virtual QString getName();
	virtual double getStartTime();
	virtual double getEndTime();
	virtual QString getNotes();
	virtual int getRunIndex();
};

}; //namespace Picto
#endif