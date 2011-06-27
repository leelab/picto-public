#ifndef _COMPARETOPARAMEXPRESSION_H_
#define _COMPARETOPARAMEXPRESSION_H_

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include "PredicateExpression.h"

namespace Picto {

/*! \brief A predicate expression involves a predicate and 2 values
 *
 *	A CompareToParamExpression gets evaluated to a boolean value.  It should be
 *	noted that the predicates and predicate expressions default to false.
 *	For example, if you tried to evaluate a predicate expression without
 *	having first set the left hand side value, it will automatically
 *	return false (as opposed to returning an error flag)
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API CompareToParamExpression : public PredicateExpression
#else
class CompareToParamExpression : public PredicateExpression
#endif
{
	Q_OBJECT
public:
	CompareToParamExpression();
	CompareToParamExpression(QString predicateName);
	~CompareToParamExpression() {};

	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new CompareToParamExpression());};

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
