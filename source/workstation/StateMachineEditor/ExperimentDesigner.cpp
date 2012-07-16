#include "ExperimentDesigner.h"
#include "ExperimentDesigner.h"
#include "../../common/memleakdetect.h"
ExperimentDesigner::ExperimentDesigner(QWidget *parent) :
	Designer(parent)
{
}

ExperimentDesigner::~ExperimentDesigner()
{
}

void ExperimentDesigner::setOpenAsset(int assetId)
{
	topAsset_->getStateMachineEditorData()->setOpenedAssetId(assetId);
}

QSharedPointer<DataStore> ExperimentDesigner::createTopLevelAsset()
{
	topAsset_ = PictoData::Create().staticCast<PictoData>();
	return topAsset_.staticCast<DataStore>();
}