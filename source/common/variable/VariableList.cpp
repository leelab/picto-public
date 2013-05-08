#include "VariableList.h"
#include "../memleakdetect.h"

namespace Picto {

VariableList::VariableList()
: Variable()
{
	list_.clear();
	AddDefinableProperty(QVariant::List,"Value",QVariant());
}

QSharedPointer<Asset> VariableList::Create()
{
	return QSharedPointer<Asset>(new VariableList());
}

void VariableList::reset()
{
	Variable::reset();
	list_ = propertyContainer_->getPropertyValue("Value").toList();
}

int VariableList::length()
{
	return list_.length();
}

void VariableList::append(QVariant value)
{
	list_.append(value);
	propertyContainer_->setPropertyValue("Value",list_);

}

void VariableList::prepend(QVariant value)
{
	list_.prepend(value);
	propertyContainer_->setPropertyValue("Value",list_);
}

void VariableList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
	propertyContainer_->setPropertyValue("Value",list_);
}

void VariableList::fromArray(QVariantList array)
{
	list_ = array;
	propertyContainer_->setPropertyValue("Value",list_);
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
	propertyContainer_->setPropertyValue("Value",list_);
}

void VariableList::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
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
	propertyContainer_->setPropertyValue("Value",list_);
	return returnVal;
}

}; //namespace Picto
