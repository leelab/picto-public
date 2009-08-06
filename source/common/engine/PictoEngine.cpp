/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */

#include "PictoEngine.h"

namespace Picto {
	namespace Engine {

PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	bExclusiveMode_(false)
{
}

void PictoEngine::setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType)
{
	timingType_ = _timingType;
}

void PictoEngine::beginExclusiveMode()
{
	/*! \todo Implement Me */
}

void PictoEngine::endExclusiveMode()
{
	/*! \todo Implement Me */
}

	}; //namespace Engine
}; //namespace Picto
