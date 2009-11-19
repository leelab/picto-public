#include "PCMAuralTarget.h"

namespace Picto {

PCMAuralTarget::PCMAuralTarget()
{
}

QSharedPointer<MixingSample> PCMAuralTarget::generateMixingSample()
{
	QSharedPointer<PCMMixingSample> pcmMixingSample(new PCMMixingSample());

	return pcmMixingSample;
}

}; //namespace Picto
