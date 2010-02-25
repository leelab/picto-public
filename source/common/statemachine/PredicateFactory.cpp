#include "PredicateFactory.h"

#include "PredicateEqual.h"
#include "PredicateNotEqual.h"
#include "PredicateGreaterThan.h"
#include "PredicateGreaterThanEq.h"
#include "PredicateGreaterThan.h"
#include "PredicateLessThan.h"
#include "PredicateLessThanEq.h"


namespace Picto
{
QSharedPointer<Predicate> PredicateFactory::createPredicate(QString predicateName)
{
	//The predicate names should match the values returned by Predicate::name()
	if(predicateName=="Equal")
		return QSharedPointer<Predicate>(new PredicateEqual);
	else if(predicateName=="Not equal")
		return QSharedPointer<Predicate>(new PredicateNotEqual);
	else if(predicateName=="Greater than")
		return QSharedPointer<Predicate>(new PredicateGreaterThan);
	else if(predicateName=="Greater than or equal")
		return QSharedPointer<Predicate>(new PredicateGreaterThanEq);
	else if(predicateName=="Less than")
		return QSharedPointer<Predicate>(new PredicateLessThan);
	else if(predicateName=="Less than or equal")
		return QSharedPointer<Predicate>(new PredicateLessThanEq);
	else
	{
		//If you hit this, one of two things has occurred:
		//	1. There's a typo in the predicateName operator
		//	2. You're trying to create a predicate that isn't in the list
		//	   so you need to modify the case statement
		Q_ASSERT(false);
		return QSharedPointer<Predicate>();
	}
	
}

} //namespace Picto