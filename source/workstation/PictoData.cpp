#include "PictoData.h"
#include "../common/storage/ObsoleteAsset.h"
#include "../common/analysis/AnalysisContainer.h"
#include "../common/memleakdetect.h"
namespace Picto
{

PictoData::PictoData()
{
	AddDefinableProperty("Name","Untitled");
	AddDefinableObjectFactory("Experiment",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(Experiment::Create))));
	AddDefinableObjectFactory("AnalysisContainer",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AnalysisContainer::Create))));
	
	//Add Obsolete Asset Factories
	AddDefinableObjectFactory("StateMachineEditorData",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

void PictoData::postDeserialize()
{
	DataStore::postDeserialize();
	//If this PictoData has any StateMachineEditorData child assets, this object should be set as edited.
	//ideally, the ObsoleteAsset child would just set itself deleted and that would
	//propogate its edited state to here, the ObsoleteAsset can't though because 
	//the latest it can do that is in postDeserialize, and this PictoData has its
	//edited flag set back to false after that point.  This should be fixed at some point.
	QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren("StateMachineEditorData");
	if(tagChildren.size())
		emit edited();
}

QSharedPointer<Asset> PictoData::Create()
{
	QSharedPointer<Asset> newPictoData(new PictoData());
	newPictoData->setSelfPtr(newPictoData);
	return newPictoData;
}

QSharedPointer<Experiment> PictoData::getExperiment()
{
	QList<QSharedPointer<Asset>> expList = getGeneratedChildren("Experiment");
	return expList.first().staticCast<Experiment>();
}

bool PictoData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto