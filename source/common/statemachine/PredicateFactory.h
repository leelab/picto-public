#ifndef _PREDICATE_FACTORY_H_
#define _PREDICATE_FACTORY_H_

#include "../common.h"
#include "predicate.h"


namespace Picto {

/*! \brief A factory for predicate objects
 *
 *	This is a really simple factory for predicate objects.  Rather than 
 *	Having the factory register predicates, the factory is assumed to
 *	"know" about all the different types of predicates.  This is done to
 *	simplify usage of the factory, and can be justified based on the fact 
 *	that there are a limited number of predicates.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API PredicateFactory
#else
class PredicateFactory
#endif
{
public:
	//The constructor is responsible for:
	//	- setting the string value
	//	- creating the icon_ QImage
	PredicateFactory() {};
	~PredicateFactory() {};

	static QSharedPointer<Predicate> createPredicate(QString predicateName);
};
} //namespace Picto

#endif