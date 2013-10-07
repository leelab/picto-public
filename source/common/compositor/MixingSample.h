#ifndef _MIXINGSAMPLE_H_
#define _MIXINGSAMPLE_H_

#include <QSharedPointer>
#include "../stimuli/PreloadedSound.h"
#include "../common.h"

namespace Picto {

/*! \brief Audio samples used by the AuralTargets
 *
 *	This hasn't been implemented yet.  I believe the intention was (as
 *	described by a classic SAT-style analogy):
 *		VisualTarget : CompositingSurface :: AuralTarget : MixingSample
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS MixingSample : public QObject
#else
class MixingSample : public QObject
#endif
{
	Q_OBJECT
public:
	MixingSample();
	virtual ~MixingSample(){};
	virtual QString getTypeName() = 0;
	virtual void setSoundEffect(QSharedPointer<PreloadedSound> soundEffect) = 0;
	virtual void scheduleVolumeChange(float volume) = 0;
	virtual void scheduleStart() = 0;
	virtual void scheduleStop() = 0;
signals:
	void playing(QString mixSampleType);
	void stopping(QString mixSampleType);
};


}; //namespace Picto

#endif
