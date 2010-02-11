/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_LESS_THAN_EQ_H_
#define _PREDICATE_LESS_THAN_EQ_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateLessThanEq : public Predicate
#else
class PredicateLessThanEq : public Predicate
#endif
{
public:
	PredicateLessThanEq();
	~PredicateLessThanEq() {};

	bool evaluate(double LHS, double RHS);

private:
};


}; //namespace Picto

#endif
