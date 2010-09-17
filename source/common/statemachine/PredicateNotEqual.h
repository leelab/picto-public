#ifndef _PREDICATE_NOT_EQUAL_H_
#define _PREDICATE_NOT_EQUAL_H_

#include "../common.h"
#include "Predicate.h"
#include "../parameter/parameter.h"

namespace Picto {

/*! \brief The not equal (!=) predicate
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateNotEqual : public Predicate
#else
class PredicateNotEqual : public Predicate
#endif
{
public:
	PredicateNotEqual();
	~PredicateNotEqual() {};

	bool evaluate(Parameter &LHS, Parameter &RHS);
	bool evaluate(Parameter &LHS, QVariant RHS);
private:
};


}; //namespace Picto

#endif
