#include "PictoData.h"
namespace Picto
{

PictoData::PictoData()
{
	AddDefinableObjectFactory("Experiment",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(Experiment::Create))));
	AddDefinableObjectFactory("StateMachineEditorData",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(StateMachineEditorData::Create))));
}

void PictoData::postSerialize()
{
	DataStore::postSerialize();
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
QSharedPointer<StateMachineEditorData> PictoData::getStateMachineEditorData()
{
	QList<QSharedPointer<Asset>> expList = getGeneratedChildren("StateMachineEditorData");
	return expList.first().staticCast<StateMachineEditorData>();
}

bool PictoData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto