#ifndef _PREDICATEEXPRESSION_H_
#define _PREDICATEEXPRESSION_H_

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include "../common.h"
#include "../statemachine/Result.h"
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
class PICTOLIB_API PredicateExpression : public Result
#else
class PredicateExpression : public UIEnabled
#endif
{
	Q_OBJECT
public:
	PredicateExpression();
	PredicateExpression(QString predicateName);
	~PredicateExpression() {};

	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new PredicateExpression());};

	bool isValid();
	bool evaluate();
	
	//Name
	void setOrder(int order);
	int order();
	
	QString toString(bool useLHSName=false, bool useRHSName=false);
	QImage toQImage(bool useLHSName=false, bool useRHSName=false);

	void setPredicate(QString predicateName) { predicateName_ = predicateName; };
	void setLHS(QString parameterName) { LHSParamName_ = parameterName; };
 	void setRHSParam(QString parameterName) { RHSParamName_ = parameterName; useRHSVal_=false;};
	void setRHSValue(QVariant RHS) { RHSval_ = RHS; useRHSVal_=true;};

	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual QString assetType(){return "PredicateExpression";};

protected:
	virtual QString defaultTagName(){return "Condition";};
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QString RHSParamName_;
	QVariant RHSval_;
	bool useRHSVal_;

private:
	QString predicateName_;
	QString LHSParamName_;

	//ParameterContainer *parameters_;
};


}; //namespace Picto

#endif
