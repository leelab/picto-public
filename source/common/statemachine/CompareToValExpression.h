#ifndef _COMPARETOVALEXPRESSION_H_
#define _COMPARETOVALEXPRESSION_H_

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include "PredicateExpression.h"

namespace Picto {

/*! \brief A predicate expression involves a predicate and 2 values
 *
 *	A CompareToValExpression gets evaluated to a boolean value.  It should be
 *	noted that the predicates and predicate expressions default to false.
 *	For example, if you tried to evaluate a predicate expression without
 *	having first set the left hand side value, it will automatically
 *	return false (as opposed to returning an error flag)
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API CompareToValExpression : public PredicateExpression
#else
class CompareToValExpression : public PredicateExpression
#endif
{
	Q_OBJECT
public:
	CompareToValExpression();
	CompareToValExpression(QString predicateName);
	~CompareToValExpression() {};

	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new CompareToValExpression());};

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
