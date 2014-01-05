#include "AnalysisVariableList.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an AnalysisVariableList.
 *	\details Adds a "Value" property to hold the list data.
 */
AnalysisVariableList::AnalysisVariableList()
: AnalysisVariable()
{
	list_.clear();
	AddDefinableObjectFactory("Value",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

/*! \brief Creates a new AnalysisVariableList and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisVariableList::Create()
{
	return QSharedPointer<Asset>(new AnalysisVariableList());
}

/*! \brief Called when this object enters scope.  Clears the contents of its underlying list.*/
void AnalysisVariableList::enteredScope()
{
	AnalysisVariable::enteredScope();
	list_.clear();
}

/*! \brief Returns the number of values in this list.*/
int AnalysisVariableList::length()
{
	return list_.length();
}

/*! \brief Adds a value to the end of this list.*/
void AnalysisVariableList::append(QVariant value)
{
	list_.append(value);
}

/*! \brief Adds a value to the beginning of this list.*/
void AnalysisVariableList::prepend(QVariant value)
{
	list_.prepend(value);
}

/*! \brief Sets the value at the input index to the input value.  If the list isn't big enough, nothing happens.*/
void AnalysisVariableList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
}

/*! \brief Sets the contents of this AnalysisVariableList to those of the input QVariantList.
 *	\details When this function is called from a script environment, a javascripot array
 *	can be input and this lists will be set to have the contents of the input array.
 */
void AnalysisVariableList::fromArray(QVariantList array)
{
	list_ = array;
}

/*! \brief Removes the first value in the list.*/
void AnalysisVariableList::removeFirst()
{
	removeAt(0);
}

/*! \brief Removes the last value in the list.*/
void AnalysisVariableList::removeLast()
{
	removeAt(list_.size()-1);
}

/*! \brief Removes the value at the input index in the list.*/
void AnalysisVariableList::removeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return;
	list_.removeAt(index);
}

/*! \brief Clears the list.*/
void AnalysisVariableList::clear()
{
	list_.clear();
}

void AnalysisVariableList::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisVariableList::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Returns the value at the input index of the list as a variant.*/
QVariant AnalysisVariableList::getValue(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	return list_[index];
}

/*! \brief Returns the first value in the list as a variant.*/
QVariant AnalysisVariableList::first()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.first();
}
/*! \brief Returns the last value in the list as a variant.*/
QVariant AnalysisVariableList::last()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.last();
}

/*! \brief Returns the object's underlying QVariantList.
 */
QVariantList AnalysisVariableList::toArray()
{
	return list_;
}

/*! \brief Returns the first value in the list and removes it from the list.*/
QVariant AnalysisVariableList::takeFirst()
{
	return takeAt(0);
}

/*! \brief Returns the last value in the list and removes it from the list.*/
QVariant AnalysisVariableList::takeLast()
{
	return takeAt(list_.size()-1);
}

/*! \brief Returns the value of the list at the input index and removes it from the list.*/
QVariant AnalysisVariableList::takeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	QVariant returnVal = list_.takeAt(index);
	return returnVal;
}

}; //namespace Picto
