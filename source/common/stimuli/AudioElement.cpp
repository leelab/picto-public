#include "AudioElement.h"
#include "../memleakdetect.h"

namespace Picto {

AudioElement::AudioElement()
{
}

QSharedPointer<Asset> AudioElement::Create()
{
	return QSharedPointer<Asset>(new AudioElement());
}

}; //namespace Picto
