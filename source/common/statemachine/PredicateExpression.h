/*! \brief A predicate expression involves a predicate and 2 values
 *
 *	A PredicateExpression gets evaluated to a boolean value.  It should be
 *	noted that the predicates and predicate expressions default to false.
 *	For example, if you tried to evaluate a predicate expression without
 *	having first set the left hand side value, it will automatically
 *	return false (as opposed to returning an error flag)
 */

#ifndef _PREDICATEEXPRESSION_H_
#define _PREDICATEEXPRESSION_H_

#include "../common.h"
#include "Predicate.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateExpression
#else
class PredicateExpression
#endif
{
public:
	PredicateExpression();
	PredicateExpression(Predicate *p);
	PredicateExpression(Predicate *p, double LHSvalue, double RHSvalue, 
		QString LHSname="", QString RHSname="");
	~PredicateExpression();

	bool isValid();
	bool evaluate();

	QString toString(bool useLHSName=false, bool useRHSName=false);
	QImage toQImage(bool useLHSName=false, bool useRHSName=false);

	void setPredicate(Predicate *p) { predicate_ = p; };
	void setLHS(double val, QString name = "") { LHSinitialized_ = true;  
												 LHSvalue_ = val; 
												 LHSname_ = name; };
	void setRHS(double val, QString name = "") { RHSinitialized_ = true; 
												 RHSvalue_ = val; 
												 RHSname_ = name;  };

	Predicate* getPredicate() { return predicate_; };
	double getLHSvalue() {return LHSvalue_; };
	double getRHSvalue() { return RHSvalue_; };
	QString getLHSname() { return LHSname_; }
	QString getRHSname() { return RHSname_; }

private:
	double LHSvalue_, RHSvalue_;
	QString LHSname_, RHSname_;
	bool LHSinitialized_, RHSinitialized_;
	Predicate *predicate_;
};


}; //namespace Picto

#endif
