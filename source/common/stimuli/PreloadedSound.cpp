#include "PreloadedSound.h"
#include "MultiPlatformSound.h"
#include "../memleakdetect.h"

namespace Picto {

QSharedPointer<PreloadedSound> (*PreloadedSound::constructFunc_)(QString) = MultiplatformSound::createMultiplatformSound;

/*! \brief Sets the constructor that will be used to generate a PreloadedSound inside createSound().
 *	\details This is important because sound turns out to be one of those things that varies pretty
 *	significantly over different platforms and run environments.  Currently, due to the
 *	lack of an event loop in the Director's experimental run environment, some of the simpler types
 *	of sound objects that we can use in the workstation, won't work in the director.  In both cases; 
 *	however, we want AudioFileParameter objects to be able to use createSound() to create a valid
 *	PreloadedSound.  This setSoundConstructor function lets us handle this case by setting a constructor for
 *	a different kind of underlying class based on which application we are running, Director or Workstation.
 */
void PreloadedSound::setSoundConstructor(QSharedPointer<PreloadedSound> (*constructFunc)(QString))
{
	constructFunc_ = constructFunc;
}

/*! \brief Generates and returns a pointer to a PreloadedSound object containing the sound located at the input path on disk.
 *	\note PreloadedSounds should only be created by using this static function.  Direct constructor's should not be
 *	used.
 *	\sa setSoundConstructor()
 */
QSharedPointer<PreloadedSound> PreloadedSound::createSound(QString path)
{
	if(constructFunc_ == NULL)
		return QSharedPointer<PreloadedSound>();
	return constructFunc_(path);
}


}; //namespace Picto
