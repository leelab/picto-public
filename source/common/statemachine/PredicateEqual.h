/*! \brief The greater than predicate
 */

#ifndef _PREDICATE_EQUAL_H_
#define _PREDICATE_EQUAL_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateEqual : public Predicate
#else
class PredicateEqual : public Predicate
#endif
{
public:
	PredicateEqual();
	~PredicateEqual() {};

	bool evaluate(Parameter &LHS, Parameter &RHS);
	bool evaluate(Parameter &LHS, QVariant RHS);

private:
};


}; //namespace Picto

#endif
