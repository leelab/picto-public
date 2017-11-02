#include <QDebug>
#include "DataViewElement.h"
#include "../memleakdetect.h"

namespace Picto
{
DataViewElement::DataViewElement()
{
	EXP_LINK_FACTORY_CREATION;

	AddDefinableProperty(QVariant::String, "ViewTitle", "View Element");
	AddDefinableProperty(PropertyContainer::viewPropertyTypeId(), "DefaultPosition",
		QVariant::fromValue(ViewProperties(DataViewSize::VIEW_SIZE_1x1, -1, -1)));
	AddDefinableProperty(QVariant::String, "InitializationScript", "");

	static int index = 0;
	dveNum_ = index++;
}

DataViewElement::~DataViewElement()
{
}

void DataViewElement::postDeserialize()
{
	ContainerElement::postDeserialize();
	setTitle(propertyContainer_->getPropertyValue("ViewTitle").toString());
}

DataViewSize::ViewSize DataViewElement::getDefaultViewSize() const
{
	//We add one to the value because ViewSizes are enumerated from 1 for ease of size-calculation.
	ViewProperties vc = propertyContainer_->getPropertyValue("DefaultPosition").value<ViewProperties>();
	return vc.size_;
}

ViewProperties DataViewElement::getDefaultViewProperties() const
{
	return propertyContainer_->getPropertyValue("DefaultPosition").value<ViewProperties>();
}

void DataViewElement::initView()
{
	setTitle(propertyContainer_->getPropertyValue("ViewTitle").toString());
}

void DataViewElement::postLinkUpdate()
{
	if (!getLinkedAsset().isNull())
	{
		setTaskConfig(getLinkedAsset()->getTaskConfig());
	}
}

bool DataViewElement::hasScripts()
{
	if (ContainerElement::hasScripts())
		return true;
	return (propertyContainer_->getPropertyValue("InitializationScript").toString() != "");
}

QMap<QString, QString> DataViewElement::getScripts()
{
	QMap<QString, QString> scripts = ContainerElement::getScripts();
	if (!propertyContainer_->getPropertyValue("InitializationScript").toString().isEmpty())
		scripts[getName().simplified().remove(' ')] = QString("InitializationScript");
	return scripts;
}

void DataViewElement::setManualTitle(const QString &newTitle)
{
	propertyContainer_->setPropertyValue("ViewTitle", newTitle);
}


};