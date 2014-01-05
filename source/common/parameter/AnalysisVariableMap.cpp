#include "AnalysisVariableMap.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an AnalysisVariableMap.
 *	\details Adds a "Value" property to hold the map data.
 */
AnalysisVariableMap::AnalysisVariableMap()
: AnalysisVariable()
{
	map_.clear();
	AddDefinableObjectFactory("Value",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

/*! \brief Creates a new AnalysisVariableMap and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisVariableMap::Create()
{
	return QSharedPointer<Asset>(new AnalysisVariableMap());
}

/*! \brief Called when this object enters scope.  Clears the contents of its underlying map.*/
void AnalysisVariableMap::enteredScope()
{
	AnalysisVariable::enteredScope();
	map_.clear();
}

/*! \brief Returns the number of values in this map.*/
int AnalysisVariableMap::length()
{
	return map_.size();
}

/*! \brief Sets the value under the input key to the input value.  If the key is already in the map, its value is replaced.
*/
void AnalysisVariableMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
}
/*! \brief Resets the map from the input Variant map.
 *	\details When used in javascript, this object is reset from the input javascript Associative Array object.  
 *	\note This cannot be used with toAssocArray as a way of storing Javascript objects since function components of the object are not usable after the transfer.
 */
void AnalysisVariableMap::fromAssocArray(QVariantMap assocArray)
{
	//Is this a problem because the keys might not be strings?
	map_ = assocArray;
}
/*! \brief Returns a variant list of keys defined in the map.
 *	\details If called from javascript, returns a javascript array containing the keys.
 */
QVariantList AnalysisVariableMap::getKeys()
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
void AnalysisVariableMap::removeAt(QVariant key)
{
	map_.take(key.toString());
}
/*! \brief Clears the map.*/
void AnalysisVariableMap::clear()
{
	map_.clear();
}


void AnalysisVariableMap::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisVariableMap::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}
/*! \brief Returns the value under the input key as a variant.*/
QVariant AnalysisVariableMap::getValue(QVariant key)
{
	QString keyString = key.toString();
	if(!map_.contains(keyString))
		return QVariant();
	return map_.value(keyString);
}

/*! \brief Returns the value under the input key as a variant and removes it from the map.*/
QVariant AnalysisVariableMap::takeAt(QVariant key)
{
	QVariant returnVal = map_.take(key.toString());
	return returnVal;
}

}; //namespace Picto
