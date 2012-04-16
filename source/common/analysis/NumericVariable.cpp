#include "NumericVariable.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

NumericVariable::NumericVariable()
{
	AddDefinableProperty(QVariant::Double,"InitValue",0);
	setValid(true);
	value_ = 0;
}

NumericVariable::~NumericVariable()
{
}

QSharedPointer<Asset> NumericVariable::Create()
{
	return QSharedPointer<Asset>(new NumericVariable());
}

void NumericVariable::reset()
{
	value_ = propertyContainer_->getPropertyValue("InitValue").toDouble();
}

void NumericVariable::postDeserialize()
{
	AnalysisTool::postDeserialize();
	reset();
}

bool NumericVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTool::validateObject(xmlStreamReader))
		return false;
	return true;
}