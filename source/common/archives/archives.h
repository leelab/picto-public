/*!	\file
 *	\brief Unimplemented.  The archive classes will be used to store data long term on disk
 *
 *	It is expected that we will need to be able to store everything associated with each
 *	experiment on the server.  The archive class should be used for helping to keep track
 *	of everything.  (At least this was my understanding.)  At the moment though, it is being
 *	used as a testbed for some compression code.
 */

#ifndef _ARCHIVES_H_
#define _ARCHIVES_H_

#include "../common.h"

namespace Picto {

PICTOLIB_API void testZip();
PICTOLIB_API void testUnZip();

}; //namespace Picto

#endif
