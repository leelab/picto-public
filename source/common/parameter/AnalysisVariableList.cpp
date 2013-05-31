#include "AnalysisVariableList.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVariableList::AnalysisVariableList()
: AnalysisVariable(),
settingValueProp_(false)
{
	list_.clear();
	AddDefinableProperty(QVariant::List,"Value",QVariant());
}

QSharedPointer<Asset> AnalysisVariableList::Create()
{
	return QSharedPointer<Asset>(new AnalysisVariableList());
}

int AnalysisVariableList::length()
{
	return list_.length();
}

void AnalysisVariableList::append(QVariant value)
{
	list_.append(value);
	copyListToValueProp();

}

void AnalysisVariableList::prepend(QVariant value)
{
	list_.prepend(value);
	copyListToValueProp();
}

void AnalysisVariableList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
	copyListToValueProp();
}

void AnalysisVariableList::fromArray(QVariantList array)
{
	list_ = array;
	copyListToValueProp();
}

void AnalysisVariableList::removeFirst()
{
	removeAt(0);
}

void AnalysisVariableList::removeLast()
{
	removeAt(list_.size()-1);
}

void AnalysisVariableList::removeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return;
	list_.removeAt(index);
	copyListToValueProp();
}

void AnalysisVariableList::postDeserialize()
{
	AnalysisVariable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
	connect(propertyContainer_->getProperty("Value").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
}

bool AnalysisVariableList::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

QVariant AnalysisVariableList::getValue(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	return list_[index];
}

QVariant AnalysisVariableList::first()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.first();
}
QVariant AnalysisVariableList::last()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.last();
}

QVariantList AnalysisVariableList::toArray()
{
	return list_;
}

QVariant AnalysisVariableList::takeFirst()
{
	return takeAt(0);
}

QVariant AnalysisVariableList::takeLast()
{
	return takeAt(list_.size()-1);
}

QVariant AnalysisVariableList::takeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	QVariant returnVal = list_.takeAt(index);
	copyListToValueProp();
	return returnVal;
}

void AnalysisVariableList::copyListToValueProp()
{
	settingValueProp_ = true;
	propertyContainer_->setPropertyValue("Value",list_);
	settingValueProp_ = false;
}

void AnalysisVariableList::copyValuePropToList()
{
	list_ = propertyContainer_->getPropertyValue("Value").toList();
}

void AnalysisVariableList::propValueChanged(Property*,QVariant)
{
	//If the change happened because of one of this object's functions, return.
	if(settingValueProp_)
		return;
	//The change must have happened from an outside source (ie. The value was reset to the initValue
	//or this is a slave and something changed on the master)
	//Set our list from the Value property
	copyValuePropToList();
}

}; //namespace Picto
