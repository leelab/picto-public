#ifndef _BLOCKGENERATOR_H_
#define _BLOCKGENERATOR_H_

#include "../common.h"

namespace Picto {

/*!	\brief Unimplemented. 
 *
 *	Originally, we had talked about generating blocks of trials.  Instead, each
 *	experiment is run by nothing more than another hierarchical state machine, 
 *	so this is unneccessary.
 */
class BlockGenerator
{
public:
	BlockGenerator();
};


}; //namespace Picto

#endif
