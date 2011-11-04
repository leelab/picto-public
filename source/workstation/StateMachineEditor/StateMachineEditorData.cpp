#include "StateMachineEditorData.h"
#include "../common/statemachine/uienabled.h"
#include "../pictodata.h"

namespace Picto
{

StateMachineEditorData::StateMachineEditorData()
{
	AddDefinableProperty(QVariant::String,"OpenedBlock","");
}

void StateMachineEditorData::setOpenedAsset(QSharedPointer<Asset> openedAsset)
{
	Q_ASSERT(openedAsset.dynamicCast<UIEnabled>());
	propertyContainer_->setPropertyValue("OpenedBlock",openedAsset.staticCast<UIEnabled>()->getUniqueId().toString());
}

QSharedPointer<Asset> StateMachineEditorData::getOpenedAsset()
{
	QSharedPointer<Asset> rootAsset = getParentAsset();
	rootAsset = rootAsset.staticCast<PictoData>()->getExperiment();
	Q_ASSERT(!rootAsset.isNull());
	QUuid openID(propertyContainer_->getPropertyValue("OpenedBlock").toString());
	if(openID.isNull())
		return rootAsset;
	QSharedPointer<Asset> savedAsset = findAssetWithID(rootAsset,openID);	
	if(savedAsset.isNull())
		return rootAsset;
	return savedAsset;
}
void StateMachineEditorData::setOpenedAssetId(QUuid openedAsset)
{
	propertyContainer_->setPropertyValue("OpenedBlock",openedAsset.toString());
}

QUuid StateMachineEditorData::getOpenedAssetId()
{
	QUuid openID(propertyContainer_->getPropertyValue("OpenedBlock").toString());
	return openID;
}

void StateMachineEditorData::postDeserialize()
{
	DataStore::postDeserialize();
}


bool StateMachineEditorData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

QSharedPointer<Asset> StateMachineEditorData::findAssetWithID(QSharedPointer<Asset> root, QUuid id)
{
	QSharedPointer<Asset> result;
	QSharedPointer<UIEnabled> uienabled = root.dynamicCast<UIEnabled>();
	if(uienabled.isNull())
		return result;
	if(!uienabled.isNull() && (uienabled->getUniqueId() == id))
	{	//This is the asset with that id, return it.
		return root;
	}

	//Recursively call the function on all of this assets children
	QStringList childTypes = uienabled->getDefinedChildTags();
	foreach(QString childType, childTypes)
	{
		QList<QSharedPointer<Asset>> assets = uienabled->getGeneratedChildren(childType);
		foreach(QSharedPointer<Asset> childAsset,assets)
		{
			result = findAssetWithID(childAsset,id);
			if(!result.isNull())
				return result;
		}
	}
	//Either the asset didn't have children or none had this id, return null pointer.
	return QSharedPointer<Asset>();
}
}//namespace Picto