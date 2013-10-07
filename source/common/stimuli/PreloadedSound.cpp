#include "PreloadedSound.h"
#include "MultiPlatformSound.h"
#include "../memleakdetect.h"

namespace Picto {

QSharedPointer<PreloadedSound> (*PreloadedSound::constructFunc_)(QString) = MultiplatformSound::createMultiplatformSound;

void PreloadedSound::setSoundConstructor(QSharedPointer<PreloadedSound> (*constructFunc)(QString))
{
	constructFunc_ = constructFunc;
}
QSharedPointer<PreloadedSound> PreloadedSound::createSound(QString path)
{
	if(constructFunc_ == NULL)
		return QSharedPointer<PreloadedSound>();
	return constructFunc_(path);
}


}; //namespace Picto
