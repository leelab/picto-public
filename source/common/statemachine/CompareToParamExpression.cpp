#include "CompareToParamExpression.h"
#include "PredicateFactory.h"

#include <QFont>
#include <QPainter>

namespace Picto {


CompareToParamExpression::CompareToParamExpression()
: PredicateExpression()
{
	AddDefinableProperty(QVariant::String,"RightParameter","");
}

CompareToParamExpression::CompareToParamExpression(QString predicateName)
: PredicateExpression(predicateName)
{
	AddDefinableProperty(QVariant::String,"RightParameter","");
	initializePropertiesToDefaults();
}

void CompareToParamExpression::postSerialize()
{
	PredicateExpression::postSerialize();
	useRHSVal_ = false;
	RHSParamName_ = propertyContainer_->getPropertyValue("RightParameter").toString();
}

bool CompareToParamExpression::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!PredicateExpression::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
