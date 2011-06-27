#include "CompareToValExpression.h"
#include "PredicateFactory.h"

#include <QFont>
#include <QPainter>

namespace Picto {


CompareToValExpression::CompareToValExpression()
: PredicateExpression()
{
	AddDefinableProperty(QVariant::String,"Value","");
}

CompareToValExpression::CompareToValExpression(QString predicateName)
: PredicateExpression(predicateName)
{
	AddDefinableProperty(QVariant::String,"Value","");
	initializePropertiesToDefaults();
}

void CompareToValExpression::postSerialize()
{
	PredicateExpression::postSerialize();
	useRHSVal_ = true;
	RHSval_ = propertyContainer_->getPropertyValue("Value").toString();
}

bool CompareToValExpression::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!PredicateExpression::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
