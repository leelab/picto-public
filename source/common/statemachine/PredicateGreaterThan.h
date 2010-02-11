/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_GREATER_THAN_H_
#define _PREDICATE_GREATER_THAN_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateGreaterThan : public Predicate
#else
class PredicateGreaterThan : public Predicate
#endif
{
public:
	PredicateGreaterThan();
	~PredicateGreaterThan() {};

	bool evaluate(double LHS, double RHS);

private:
};


}; //namespace Picto

#endif
