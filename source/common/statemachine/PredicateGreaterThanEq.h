#ifndef _PREDICATE_GREATER_THAN_EQ_H_
#define _PREDICATE_GREATER_THAN_EQ_H_

#include "../common.h"
#include "Predicate.h"
#include "../parameter/parameter.h"

namespace Picto {

/*! \brief The greater than or equal to (>=) predicate
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateGreaterThanEq : public Predicate
#else
class PredicateGreaterThanEq : public Predicate
#endif
{
public:
	PredicateGreaterThanEq();
	~PredicateGreaterThanEq() {};

	bool evaluate(Parameter &LHS, Parameter &RHS);
	bool evaluate(Parameter &LHS, QVariant RHS);

private:
};


}; //namespace Picto

#endif
