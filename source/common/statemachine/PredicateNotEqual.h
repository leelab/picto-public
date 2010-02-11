/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_NOT_EQUAL_H_
#define _PREDICATE_NOT_EQUAL_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateNotEqual : public Predicate
#else
class PredicateNotEqual : public Predicate
#endif
{
public:
	PredicateNotEqual();
	~PredicateNotEqual() {};

	bool evaluate(double LHS, double RHS);

private:
};


}; //namespace Picto

#endif
