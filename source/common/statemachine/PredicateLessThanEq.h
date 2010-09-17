#ifndef _PREDICATE_LESS_THAN_EQ_H_
#define _PREDICATE_LESS_THAN_EQ_H_

#include "../common.h"
#include "Predicate.h"
#include "../parameter/parameter.h"

namespace Picto {

/*! \brief The less than or equal (<=) predicate
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateLessThanEq : public Predicate
#else
class PredicateLessThanEq : public Predicate
#endif
{
public:
	PredicateLessThanEq();
	~PredicateLessThanEq() {};

	bool evaluate(Parameter &LHS, Parameter &RHS);
	bool evaluate(Parameter &LHS, QVariant RHS);

private:
};


}; //namespace Picto

#endif
