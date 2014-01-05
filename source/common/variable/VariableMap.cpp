#include "VariableMap.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an VariableMap.
 *	\details Adds a "Value" property to hold the map data.
 */
VariableMap::VariableMap()
: Variable(),
settingValueProp_(false)
{
	map_.clear();
	AddDefinableProperty(QVariant::Map,"Value",QVariant(map_));
}

/*! \brief Creates a new QVariantMap and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> VariableMap::Create()
{
	return QSharedPointer<Asset>(new VariableMap());
}

/*! \brief Returns the number of values in this map.*/
int VariableMap::length()
{
	return map_.size();
}

/*! \brief Sets the value under the input key to the input value.  If the key is already in the map, its value is replaced.
*/
void VariableMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
	copyMapToValueProp();
}

/*! \brief Resets the map from the input Variant map.
 *	\details When used in javascript, this object is reset from the input javascript Associative Array object.  
 *	\note This cannot be used with toAssocArray as a way of storing Javascript objects since function components of the object are not usable after the transfer.
 */
void VariableMap::fromAssocArray(QVariantMap assocArray)
{
	//Is this a problem because the keys might not be strings?
	map_ = assocArray;
	copyMapToValueProp();
}

/*! \brief Returns a variant list of keys defined in the map.
 *	\details If called from javascript, returns a javascript array containing the keys.
 */
QVariantList VariableMap::getKeys()
{
	QVariantList returnVal;
	returnVal.reserve(map_.size());
	foreach(QString key,map_.keys())
	{
		returnVal.append(key);
	}
	return returnVal;
}
	
/*! \brief Removes the input key and its value from the map.*/	
void VariableMap::removeAt(QVariant key)
{
	map_.take(key.toString());
	copyMapToValueProp();
}

/*! \brief Clears the map.*/
void VariableMap::clear()
{
	map_.clear();
	copyMapToValueProp();
}

void VariableMap::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
	connect(propertyContainer_->getProperty("Value").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
}

bool VariableMap::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Returns the value under the input key as a variant.*/
QVariant VariableMap::getValue(QVariant key)
{
	QString keyString = key.toString();
	if(!map_.contains(keyString))
		return QVariant();
	return map_.value(keyString);
}

/*! \brief Returns the value under the input key as a variant and removes it from the map.*/
QVariant VariableMap::takeAt(QVariant key)
{
	QVariant returnVal = map_.take(key.toString());
	copyMapToValueProp();
	return returnVal;
}

/*! \brief Copies the current value of the underlying QVariantMap to the "Value" Property so that
 *	it will be sent over the network and recorded.
 */
void VariableMap::copyMapToValueProp()
{
	settingValueProp_ = true;
	propertyContainer_->setPropertyValue("Value",map_);
	settingValueProp_ = false;
}

/*! \brief Copies the current value of the "Value" Property to the underlying QVariantMap to make sure it has valid data.
 *	\details The Property value gets updated on a slave when a object in the master Experiment's value changes.  It also gets reset when
 *	its parent first enters scope.  We use an underlying QVariantMap to keep "get" operations efficient, so we need to 
 *	make sure to call this function whenever the "Value" Property changes to keep the QVariantMap data valid.
 */
void VariableMap::copyValuePropToMap()
{
	map_ = propertyContainer_->getPropertyValue("Value").toMap();
}

/*! \brief Called when the "Value" Property changes and internally calls the copyValuePropToMap() function
 *	 so that the underlying QVariantMap used for efficient "get" operations will contain correct data.
 *	\sa copyValuePropToMap()
 */
void VariableMap::propValueChanged(Property*,QVariant)
{
	//If the change happened because of one of this object's functions, return.
	if(settingValueProp_)
		return;
	//The change must have happened from an outside source (ie. The value was reset to the initValue
	//or this is a slave and something changed on the master)
	//Set our Map from the Value property
	copyValuePropToMap();
}

}; //namespace Picto
