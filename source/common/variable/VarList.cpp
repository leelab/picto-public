#include "VarList.h"
#include "../memleakdetect.h"

namespace Picto {

VarList::VarList()
: Variable()
{
	list_.clear();
	AddDefinableProperty(QVariant::List,"Value",QVariant());
}

QSharedPointer<Asset> VarList::Create()
{
	return QSharedPointer<Asset>(new VarList());
}

void VarList::reset()
{
	Variable::reset();
	list_ = propertyContainer_->getPropertyValue("Value").toList();
}

int VarList::length()
{
	return list_.length();
}

void VarList::append(QVariant value)
{
	list_.append(value);
	propertyContainer_->setPropertyValue("Value",list_);

}

void VarList::prepend(QVariant value)
{
	list_.prepend(value);
	propertyContainer_->setPropertyValue("Value",list_);
}

void VarList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
	propertyContainer_->setPropertyValue("Value",list_);
}

void VarList::fromArray(QVariantList array)
{
	list_ = array;
	propertyContainer_->setPropertyValue("Value",list_);
}

QVariant VarList::getValue(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	return list_[index];
}

QVariant VarList::first()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.first();
}
QVariant VarList::last()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.last();
}

QVariantList VarList::toArray()
{
	return list_;
}

QVariant VarList::takeFirst()
{
	return takeAt(0);
}

QVariant VarList::takeLast()
{
	return takeAt(list_.size()-1);
}

QVariant VarList::takeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	QVariant returnVal = list_.takeAt(index);
	propertyContainer_->setPropertyValue("Value",list_);
	return returnVal;
}

void VarList::removeFirst()
{
	removeAt(0);
}

void VarList::removeLast()
{
	removeAt(list_.size()-1);
}

void VarList::removeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return;
	list_.removeAt(index);
	propertyContainer_->setPropertyValue("Value",list_);
}

void VarList::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
}

bool VarList::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
