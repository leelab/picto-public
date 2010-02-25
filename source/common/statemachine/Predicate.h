/*!	\brief The base class for all logic predicates (examples: ==, <. >, etc)
 *
 *	In a state machine, we are constantly evaluating logical expressions.  For
 *	example the expression "trial < 10" might be used to determine a branching path
 *	Since the state machine is GUI based, and does not involve writing code, we
 *	are using predicates, predicateExpressions, and CompoundExpressions to control
 *  these branches.
 *	
 *	The predicate class is a base class which all predicates must inherit.

 *	Possible predicates:
 *		<
 *		>
 *		<=
 *		>=
 *		==
 *		!=
 *		javascript
 */

#ifndef _PREDICATE_H_
#define _PREDICATE_H_

#include <QString>
#include <QImage>

#include "../common.h"
#include "../storage/datastore.h"
#include "../parameter/parameter.h"


namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API Predicate
#else
class Predicate
#endif
{
public:
	//The constructor is responsible for:
	//	- setting the predicateString_ value
	//	- setting the name_ value
	//	- creating the icon_ QImage
	Predicate();
	~Predicate();

	virtual bool evaluate(Parameter &LHS, Parameter &RHS) = 0;
	virtual bool evaluate(Parameter &LHS, QVariant RHS) = 0;
	QImage toQImage() { return icon_; };
	QString toString() { return predicateString_; };

	QString name() { return name_; };

protected:
	QString name_;
	QImage icon_;
	QString predicateString_;
};


}; //namespace Picto

#endif
