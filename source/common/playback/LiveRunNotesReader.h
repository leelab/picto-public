#ifndef _LiveRunNotesReader_H_
#define _LiveRunNotesReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
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