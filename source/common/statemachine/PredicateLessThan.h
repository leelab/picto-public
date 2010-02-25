/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_LESS_THAN_H_
#define _PREDICATE_LESS_THAN_H_

#include "../common.h"
#include "Predicate.h"
#include "../parameter/parameter.h"

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

	bool evaluate(Parameter &LHS, Parameter &RHS);
	bool evaluate(Parameter &LHS, QVariant RHS);

private:
};


}; //namespace Picto

#endif
