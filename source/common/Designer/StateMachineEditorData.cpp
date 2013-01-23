#include "StateMachineEditorData.h"
#include "../common/statemachine/uienabled.h"
#include "../pictodata.h"
#include "../../common/memleakdetect.h"

namespace Picto
{

StateMachineEditorData::StateMachineEditorData()
{
	AddDefinableProperty(QVariant::String,"OpenedBlock","");
}

QSharedPointer<Asset> StateMachineEditorData::Create()
{
	return QSharedPointer<Asset>(new StateMachineEditorData());
}

void StateMachineEditorData::setOpenedAsset(QSharedPointer<Asset> openedAsset)
{
	Q_ASSERT(openedAsset.dynamicCast<UIEnabled>());
	propertyContainer_->setPropertyValue("OpenedBlock",openedAsset.staticCast<UIEnabled>()->getAssetId());
}

QSharedPointer<Asset> StateMachineEditorData::getOpenedAsset()
{
	QSharedPointer<Asset> rootAsset = getParentAsset();
	rootAsset = rootAsset.staticCast<PictoData>()->getExperiment();
	Q_ASSERT(!rootAsset.isNull());
	int openID(propertyContainer_->getPropertyValue("OpenedBlock").toInt());
	if(openID == 0)
		return rootAsset;
	QSharedPointer<Asset> savedAsset = findAssetWithID(rootAsset,openID);	
	if(savedAsset.isNull())
		return rootAsset;
	return savedAsset;
}
void StateMachineEditorData::setOpenedAssetId(int openedAsset)
{
	propertyContainer_->setPropertyValue("OpenedBlock",QString::number(openedAsset));
}

int StateMachineEditorData::getOpenedAssetId()
{
	int openID(propertyContainer_->getPropertyValue("OpenedBlock").toInt());
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

QSharedPointer<Asset> StateMachineEditorData::findAssetWithID(QSharedPointer<Asset> root, int id)
{
	QSharedPointer<Asset> result;
	QSharedPointer<UIEnabled> uienabled = root.dynamicCast<UIEnabled>();
	if(uienabled.isNull())
		return result;
	if(!uienabled.isNull() && (uienabled->getAssetId() == id))
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