/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_GREATER_THAN_EQ_H_
#define _PREDICATE_GREATER_THAN_EQ_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateGreaterThanEq : public Predicate
#else
class PredicateGreaterThanEq : public Predicate
#endif
{
public:
	PredicateGreaterThanEq();
	~PredicateGreaterThanEq() {};

	bool evaluate(double LHS, double RHS);

private:
};


}; //namespace Picto

#endif
