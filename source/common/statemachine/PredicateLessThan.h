/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_LESS_THAN_H_
#define _PREDICATE_LESS_THAN_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateLessThan : public Predicate
#else
class PredicateLessThan : public Predicate
#endif
{
public:
	PredicateLessThan();
	~PredicateLessThan() {};

	bool evaluate(double LHS, double RHS);

private:
};


}; //namespace Picto

#endif
