#include "VariableList.h"
#include "../memleakdetect.h"

namespace Picto {

VariableList::VariableList()
: Variable(),
settingValueProp_(false)
{
	list_.clear();
	AddDefinableProperty(QVariant::List,"Value",QVariant(list_));
}

QSharedPointer<Asset> VariableList::Create()
{
	return QSharedPointer<Asset>(new VariableList());
}

int VariableList::length()
{
	return list_.length();
}

void VariableList::append(QVariant value)
{
	list_.append(value);
	copyListToValueProp();

}

void VariableList::prepend(QVariant value)
{
	list_.prepend(value);
	copyListToValueProp();
}

void VariableList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
	copyListToValueProp();
}

void VariableList::fromArray(QVariantList array)
{
	list_ = array;
	copyListToValueProp();
}

void VariableList::removeFirst()
{
	removeAt(0);
}

void VariableList::removeLast()
{
	removeAt(list_.size()-1);
}

void VariableList::removeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return;
	list_.removeAt(index);
	copyListToValueProp();
}

void VariableList::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
	connect(propertyContainer_->getProperty("Value").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
}

bool VariableList::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

QVariant VariableList::getValue(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	return list_[index];
}

QVariant VariableList::first()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.first();
}
QVariant VariableList::last()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.last();
}

QVariantList VariableList::toArray()
{
	return list_;
}

QVariant VariableList::takeFirst()
{
	return takeAt(0);
}

QVariant VariableList::takeLast()
{
	return takeAt(list_.size()-1);
}

QVariant VariableList::takeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	QVariant returnVal = list_.takeAt(index);
	copyListToValueProp();
	return returnVal;
}

void VariableList::copyListToValueProp()
{
	settingValueProp_ = true;
	propertyContainer_->setPropertyValue("Value",list_);
	settingValueProp_ = false;
}

void VariableList::copyValuePropToList()
{
	QVariant currVal = propertyContainer_->getPropertyValue("Value");
	list_ = currVal.toList();
	//If the variant was empty, the list gets set with a single value of zero.  Fix that.
//	if(list_.size() == 1)

}

void VariableList::propValueChanged(Property*,QVariant)
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
