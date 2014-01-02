#ifndef _OldVersionAsset_H_
#define _OldVersionAsset_H_

#include <QSharedPointer>

#include "../common.h"
#include "Asset.h"

namespace Picto {
/*!	\brief Assets that inherit from this type require the session to be reserialized and then deserialized if they exist.
 *   
 *	\details Currently, the way an OldVersionAsset indicates that it needs the session to be reserialized and then deserialized
 *	is by calling the static reportOldVersionAsset() function.  This is checked in the external system by using encounteredOldVersionAsset().
 *	We probably should have just called reportOldVersionAsset() inside the OldVersionAsset() constructor since the presence of any
 *	kind of OldVersionAsset should be enough to indicate that an auto upgrade of the experiment is necessary.
 *	We may want to consider if this will work or if it will cause problems, and make the change if it will work.
 * 
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	/*! \brief Returns true if an OldVersionAsset appeared in the experiment.  In this case an automatic upgrade (serialization followed by
	 *	deserialization) should be performed.
	 */
	static bool encounteredOldVersionAsset(){return encounteredAsset_;};
	/*! \brief Use this after using encounteredOldVersionAsset() to reset the class for future OldVersionAsset checks.
	 */
	static void clearOldVersionAssetFlag(){encounteredAsset_ = false;};

protected:
	static void reportOldVersionAsset();
private:
	static bool encounteredAsset_;
};


}; //namespace Picto

#endif
