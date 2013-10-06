#ifndef _OldVersionAsset_H_
#define _OldVersionAsset_H_

#include <QSharedPointer>

#include "../common.h"
#include "Asset.h"

namespace Picto {
/*!	\brief Assets that inherit from this type require the session to be reserialized and then deserialized if they exist.
 *   They indicate the presence of old syntax in the serialized in design file.
 * 
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API OldVersionAsset
#else
class OldVersionAsset
#endif
{
public:
	OldVersionAsset();
	virtual ~OldVersionAsset();

	static bool encounteredOldVersionAsset(){return encounteredAsset_;};
	static void clearOldVersionAssetFlag(){encounteredAsset_ = false;};

protected:
	static void reportOldVersionAsset();
private:
	static bool encounteredAsset_;
};


}; //namespace Picto

#endif
