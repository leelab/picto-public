/*!	\brief A compundExpression is made up of multiple PredicateExpressions combined with AND, OR, and NOT
 *
 *	The compoundExpression object is used to combine multiple predicate expressions
 *	into a single larger expression.  For example, the predicate expressions "x<3" and
 *	"y>5" can be combined into a more complex compound expression like: x<3 & y>5
 *	The PredicateExpression object supports AND, OR, and NOT as well as the ability 
 *	to create sub expressions like (x<3) & ( (y>5) | (y<1) ).
 *
 *	In addition to creating compound expressions from PredicateExpressions, it is also
 *	possible to create a compound expression with other compound expressions.  This allows
 *	the creation of complex expressions with parenthesis (effectively).
 *
 *	A compound expression consists of left and right sides (which can be either predicate
 *	expressions, or compound expressions).  Both sides can be inverted (the NOT operator)
 *	and the two sides are joind by AND or OR (additional operators like XOR aren't really 
 *	neccessary, since they can be created from AND, OR, and NOT).
 *
 */

#ifndef _COMPOUNTEXPRESSION_H_
#define _COMPOUNTEXPRESSION_H_

#include "../common.h"
#include "PredicateExpression.h"

namespace Picto {

namespace CompoundExpressionOperator
{
	typedef enum
	{
		and,
		or
	}CompoundExpressionOperator;
} //namespace CompoundExpressionOperator

#if defined WIN32 || defined WINCE
class PICTOLIB_API CompoundExpression
#else
class CompoundExpression
#endif
{
public:
	//CompoundExpression();

	//Yes, it's dumb to create 4 different constructors.  There's got to 
	//be a better way, but I can't think of one....
	CompoundExpression(PredicateExpression *LHS, PredicateExpression *RHS, 
				CompoundExpressionOperator::CompoundExpressionOperator op,
				bool invertLHS=false, bool invertRHS=false);
	CompoundExpression(CompoundExpression *LHS, PredicateExpression *RHS, 
				CompoundExpressionOperator::CompoundExpressionOperator op,
				bool invertLHS=false, bool invertRHS=false);
	CompoundExpression(PredicateExpression *LHS, CompoundExpression *RHS, 
				CompoundExpressionOperator::CompoundExpressionOperator op,
				bool invertLHS=false, bool invertRHS=false);
	CompoundExpression(CompoundExpression *LHS, CompoundExpression *RHS, 
				CompoundExpressionOperator::CompoundExpressionOperator op,
				bool invertLHS=false, bool invertRHS=false);

	~CompoundExpression() {};

	//There are no getter functions.  Since the operands are stored in a private
	//union, the end user wouldn't be able to interpret the returned operand

	//Setters
	void setLHSCompoundExp(CompoundExpression *exp, bool invert=false);
	void setRHSCompoundExp(CompoundExpression *exp, bool invert=false);
	void setLHSPredicateExp(PredicateExpression *exp, bool invert=false);
	void setRHSPredicateExp(PredicateExpression *exp, bool invert=false);

	void setOperator(CompoundExpressionOperator::CompoundExpressionOperator op) { operator_=op; };

	//Evaluate the expression
	bool evaluate();

	//Output functions
	QString toString(bool useLHSNames=false, bool useRHSNames=false);
	QImage toQImage(bool useLHSNames=false, bool useRHSNames=false);


private:
	typedef struct
	{
		bool isPred;
		union 
		{
			PredicateExpression *predExp;
			CompoundExpression *comExp;
		};
	}Operand;

	Operand LHS_;
	Operand RHS_;

	bool invertLHS_, invertRHS_;
	CompoundExpressionOperator::CompoundExpressionOperator operator_;
	
};


}; //namespace Picto

#endif
