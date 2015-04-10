#include "DataViewElement.h"
#include "../memleakdetect.h"

using namespace Picto;

DataViewElement::DataViewElement()
{
	AddDefinableProperty(QVariant::String, "ViewTitle", "View Element");
}

DataViewElement::~DataViewElement()
{

}

void DataViewElement::postDeserialize()
{
	ContainerElement::postDeserialize();
	connect(propertyContainer_->getProperty("ViewTitle").data(), SIGNAL(edited()), this, SLOT(nameWasEdited()));
	nameWasEdited();
}

void DataViewElement::nameWasEdited()
{
	setTitle(propertyContainer_->getPropertyValue("ViewTitle").toString());
}