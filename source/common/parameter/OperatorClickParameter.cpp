#include "OperatorClickParameter.h"

namespace Picto {

QList<QWeakPointer<OperatorClickParameter>> OperatorClickParameter::params_;

OperatorClickParameter::OperatorClickParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint(0,0));
	AddDefinableProperty(QVariant::Bool,"NewClick",false);
}

QSharedPointer<Asset> OperatorClickParameter::Create()
{
	QSharedPointer<Asset> newParam(new OperatorClickParameter());
	//Add this parameter to the list that we will use to send click info.
	params_.append(QWeakPointer<OperatorClickParameter>(newParam.staticCast<OperatorClickParameter>()));
	return newParam;
}

//Used to add an operator click to the system.
void OperatorClickParameter::addClick(QPoint point)
{
	QList<QSharedPointer<Property>> affectedProps;
	QList<QWeakPointer<OperatorClickParameter>>::iterator it;
	for(it = params_.begin();it != params_.end();)
	{
		QSharedPointer<OperatorClickParameter> sharedParam = (*it).toStrongRef();
		if(!sharedParam)
		{
			it = params_.erase(it);
			continue;
		}
		sharedParam->setLastClick(point);
		it++;
	}
}

//Sets the parameters to reflect a new click
void OperatorClickParameter::setLastClick(QPoint point)
{
	propertyContainer_->setPropertyValue("Pos",point);
	propertyContainer_->setPropertyValue("NewClick",true);
}

void OperatorClickParameter::postDeserialize()
{
	Parameter::postDeserialize();
}

bool OperatorClickParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
