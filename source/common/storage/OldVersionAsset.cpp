#include "OldVersionAsset.h"
#include "../memleakdetect.h"

namespace Picto {

bool OldVersionAsset::encounteredAsset_ = false;

OldVersionAsset::OldVersionAsset()
{
}

OldVersionAsset::~OldVersionAsset()
{
}

/*! \brief Call this to tell the OldVersionAsset class taht an OldVersionAsset was encountered and an automatic design
 *	syntax upgrade is going to need to happen.
 */
void OldVersionAsset::reportOldVersionAsset()
{
	encounteredAsset_ = true;
}

}; //namespace Picto
