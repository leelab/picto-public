#include "ParameterContainer.h"

#include <QtVariantProperty>

namespace Picto {

ParameterContainer::ParameterContainer(QString _containerName)
{
    containerGroupItem_ = variantManager_.addProperty(QtVariantPropertyManager::groupTypeId(),
													  _containerName);

	connect(&variantManager_,
		    SIGNAL(valueChanged(QtVariantProperty *, const QVariant &)),
		    this,
			SLOT(slotPropertyManagerValueChanged(QtVariantProperty *, const QVariant &))
			);
}

void ParameterContainer::setContainerName(QString _containerName)
{
	containerGroupItem_->setValue(_containerName);
}

void ParameterContainer::addParameter(Parameter _parameter)
{
    QtVariantProperty *item = variantManager_.addProperty(_parameter.type(),
														  _parameter.name());

	item->setValue(_parameter.value());

	QMap<QString, QVariant> attributes = _parameter.getAttributes();
	foreach(QString attributeName, attributes.keys())
	{
		item->setAttribute(attributeName, attributes.value(attributeName));
	}

	containerGroupItem_->addSubProperty(item);
	parameters_[_parameter.name()] = item;
}

QVariant ParameterContainer::getParameterValue(QString _parameterName)
{
	if(parameters_.contains(_parameterName))
	{
		return variantManager_.value(parameters_[_parameterName]);
	}
	else
	{
		return QVariant(QVariant::Invalid);
	}
}

void ParameterContainer::setParameterValue(QString _parameterName, QVariant _value)
{
	if(parameters_.contains(_parameterName))
	{
		parameters_[_parameterName]->setValue(_value);
	}
	else
	{
		addParameter(Parameter(_value.type(), _parameterName, _value));
	}
}

void ParameterContainer::slotPropertyManagerValueChanged(QtVariantProperty * property,
														 const QVariant & value)
{
	QMapIterator<QString, QtVariantProperty *> paramIterator(parameters_);
	while(paramIterator.hasNext())
	{
		paramIterator.next();
		if(paramIterator.value() == property)
		{
			emit signalParameterValueChanged(paramIterator.key(), value);
			break;
		}
	}
}

}; //namespace Picto
