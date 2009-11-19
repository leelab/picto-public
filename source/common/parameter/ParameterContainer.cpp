#include "ParameterContainer.h"

#include <QtProperty>

namespace Picto {

ParameterContainer::ParameterContainer(QString _containerName)
{
    containerGroupItem_ = variantManager_.addProperty(QtVariantPropertyManager::groupTypeId(),
													  _containerName);

	connect(&variantManager_,
		    SIGNAL(valueChanged(QtProperty *, const QVariant &)),
		    this,
			SLOT(slotPropertyManagerValueChanged(QtProperty *, const QVariant &))
			);
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

void ParameterContainer::slotPropertyManagerValueChanged(QtProperty * property,
														 const QVariant & value)
{
	QMapIterator<QString, QtProperty *> paramIterator(parameters_);
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
