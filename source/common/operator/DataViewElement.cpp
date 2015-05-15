#include <QDebug>
#include "DataViewElement.h"
#include "../memleakdetect.h"

namespace Picto
{
DataViewElement::DataViewElement()
{
	EXP_LINK_FACTORY_CREATION;
	
	AddDefinableProperty(QVariant::String, "ViewTitle", "View Element");
	sizeList_ << "1x1" << "2x2" << "3x3" << "4x4";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "DefaultSize", 0, "enumNames", sizeList_);
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
	return (DataViewSize::ViewSize)(propertyContainer_->getPropertyValue("DefaultSize").toInt() + 1);
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

};