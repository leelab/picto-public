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

void OldVersionAsset::reportOldVersionAsset()
{
	encounteredAsset_ = true;
}

}; //namespace Picto
