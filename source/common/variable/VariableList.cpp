#include "VariableList.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an VariableList.
 *	\details Adds a "Value" property to hold the list data.
 */
VariableList::VariableList()
: Variable(),
settingValueProp_(false)
{
	list_.clear();
	AddDefinableProperty(QVariant::List,"Value",QVariant(list_));
}

/*! \brief Creates a new VariableList and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> VariableList::Create()
{
	return QSharedPointer<Asset>(new VariableList());
}

/*! \brief Returns the number of values in this list.*/
int VariableList::length()
{
	return list_.length();
}

/*! \brief Adds a value to the end of this list.*/
void VariableList::append(QVariant value)
{
	list_.append(value);
	copyListToValueProp();

}

/*! \brief Adds a value to the beginning of this list.*/
void VariableList::prepend(QVariant value)
{
	list_.prepend(value);
	copyListToValueProp();
}

/*! \brief Sets the value at the input index to the input value.  If the list isn't big enough, nothing happens.*/
void VariableList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
	copyListToValueProp();
}

/*! \brief Sets the contents of this VariableList to those of the input QVariantList.
 *	\details When this function is called from a script environment, a javascripot array
 *	can be input and this lists will be set to have the contents of the input array.
 */
void VariableList::fromArray(QVariantList array)
{
	list_ = array;
	copyListToValueProp();
}

/*! \brief Removes the first value in the list.*/
void VariableList::removeFirst()
{
	removeAt(0);
}

/*! \brief Removes the last value in the list.*/
void VariableList::removeLast()
{
	removeAt(list_.size()-1);
}

/*! \brief Removes the value at the input index in the list.*/
void VariableList::removeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return;
	list_.removeAt(index);
	copyListToValueProp();
}

/*! \brief Clears the list.*/
void VariableList::clear()
{
	list_.clear();
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

/*! \brief Returns the value at the input index of the list as a variant.*/
QVariant VariableList::getValue(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	return list_[index];
}

/*! \brief Returns the first value in the list as a variant.*/
QVariant VariableList::first()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.first();
}

/*! \brief Returns the last value in the list as a variant.*/
QVariant VariableList::last()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.last();
}

/*! \brief Returns the object's underlying QVariantList.
 */
QVariantList VariableList::toArray()
{
	return list_;
}

/*! \brief Returns the first value in the list and removes it from the list.*/
QVariant VariableList::takeFirst()
{
	return takeAt(0);
}

/*! \brief Returns the last value in the list and removes it from the list.*/
QVariant VariableList::takeLast()
{
	return takeAt(list_.size()-1);
}

/*! \brief Returns the value of the list at the input index and removes it from the list.*/
QVariant VariableList::takeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	QVariant returnVal = list_.takeAt(index);
	copyListToValueProp();
	return returnVal;
}

/*! \brief Copies the current value of the underlying QVariantList to the "Value" Property so that
 *	it will be sent over the network and recorded.
 */
void VariableList::copyListToValueProp()
{
	settingValueProp_ = true;
	propertyContainer_->setPropertyValue("Value",list_);
	settingValueProp_ = false;
}

/*! \brief Copies the current value of the "Value" Property to the underlying QVariantList to make sure it has valid data.
 *	\details The Property value gets updated on a slave when a object in the master Experiment's value changes.  It also gets reset when
 *	its parent first enters scope.  We use an underlying QVariantList to keep "get" operations efficient, so we need to 
 *	make sure to call this function whenever the "Value" Property changes to keep the QVariantList data valid.
 */
void VariableList::copyValuePropToList()
{
	QVariant currVal = propertyContainer_->getPropertyValue("Value");
	list_ = currVal.toList();
	//If the variant was empty, the list gets set with a single value of zero.  Fix that.
//	if(list_.size() == 1)

}

/*! \brief Called when the "Value" Property changes and internally calls the copyValuePropToList() function
 *	 so that the underlying QVariantList used for efficient "get" operations will contain correct data.
 *	\sa copyValuePropToList()
 */
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
