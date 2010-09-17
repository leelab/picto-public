#ifndef _PREDICATEEXPRESSION_H_
#define _PREDICATEEXPRESSION_H_

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include "../common.h"
#include "../parameter/parameter.h"
#include "../parameter/ParameterContainer.h"
#include "Predicate.h"

namespace Picto {

/*! \brief A predicate expression involves a predicate and 2 values
 *
 *	A PredicateExpression gets evaluated to a boolean value.  It should be
 *	noted that the predicates and predicate expressions default to false.
 *	For example, if you tried to evaluate a predicate expression without
 *	having first set the left hand side value, it will automatically
 *	return false (as opposed to returning an error flag)
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PredicateExpression : public DataStore
#else
class PredicateExpression : public DataStore
#endif
{
public:
	PredicateExpression();
	PredicateExpression(QString predicateName);
	~PredicateExpression() {};

	bool isValid();
	bool evaluate();

	QString toString(bool useLHSName=false, bool useRHSName=false);
	QImage toQImage(bool useLHSName=false, bool useRHSName=false);

	void setPredicate(QString predicateName) { predicateName_ = predicateName; };
	void setLHS(QString parameterName) { LHSParamName_ = parameterName; };
 	void setRHSParam(QString parameterName) { RHSParamName_ = parameterName; useRHSVal_=false;};
	void setRHSValue(QVariant RHS) { RHSval_ = RHS; useRHSVal_=true;};

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setParameterContainer(ParameterContainer *params) { parameters_ = params; };

private:
	QString predicateName_;
	QString LHSParamName_;
	QString RHSParamName_;
	QVariant RHSval_;
	bool useRHSVal_;

	ParameterContainer *parameters_;
};


}; //namespace Picto

#endif
