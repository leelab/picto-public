/*! \file RenderingTarget.cpp
 * \ingroup picto_compositor
 * \brief
 */

#include "RenderingTarget.h"

namespace Picto {

RenderingTarget::RenderingTarget(QSharedPointer<VisualTarget> visualTarget,
								 QSharedPointer<AuralTarget> auralTarget)
{
	visualTarget_ = visualTarget;
	auralTarget_ = auralTarget;
}

QSharedPointer<CompositingSurface> RenderingTarget::generateCompositingSurface()
{
	return visualTarget_->generateCompositingSurface();
}

QSharedPointer<MixingSample> RenderingTarget::generateMixingSample()
{
	return auralTarget_->generateMixingSample();
}

}; //namespace Picto
