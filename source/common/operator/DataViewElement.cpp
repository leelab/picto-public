#include "DataViewElement.h"
#include "../memleakdetect.h"

namespace Picto
{
DataViewElement::DataViewElement()
{
	AddDefinableProperty(QVariant::String, "ViewTitle", "View Element");
	sizeList_ << "1x1" << "2x2" << "3x3" << "4x4";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "DefaultSize", 0, "enumNames", sizeList_);
}

DataViewElement::~DataViewElement()
{

}

void DataViewElement::postDeserialize()
{
	ContainerElement::postDeserialize();
	setTitle(propertyContainer_->getPropertyValue("ViewTitle").toString());

	getTaskConfig()->addObserver(this);
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

};