#ifndef _PREDICATE_GREATER_THAN_H_
#define _PREDICATE_GREATER_THAN_H_

#include "../common.h"
#include "Predicate.h"
#include "../parameter/parameter.h"

namespace Picto {

/*! \brief The greater than (>) predicate
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateGreaterThan : public Predicate
#else
class PredicateGreaterThan : public Predicate
#endif
{
public:
	PredicateGreaterThan();
	~PredicateGreaterThan() {};

	bool evaluate(Parameter &LHS, Parameter &RHS);
	bool evaluate(Parameter &LHS, QVariant RHS);

private:
};


}; //namespace Picto

#endif
