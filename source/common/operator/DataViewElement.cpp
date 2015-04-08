#include "DataViewElement.h"
#include "../memleakdetect.h"

using namespace Picto;

DataViewElement::DataViewElement()
{

}

DataViewElement::~DataViewElement()
{

}

void DataViewElement::postDeserialize()
{
	ContainerElement::postDeserialize();
	connect(propertyContainer_->getProperty("Name").data(), SIGNAL(edited()), this, SLOT(nameWasEdited()));
	nameWasEdited();
}

void DataViewElement::nameWasEdited()
{
	setTitle(propertyContainer_->getPropertyValue("Name").toString());
}