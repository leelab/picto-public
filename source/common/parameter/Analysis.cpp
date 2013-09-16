#include "Analysis.h"
#include "AnalysisDataSource.h"
#include "AnalysisFunction.h"
#include "AnalysisScriptContainer.h"
#include "AnalysisNumberVariable.h"
#include "AnalysisStringVariable.h"
#include "AnalysisVariableMap.h"
#include "AnalysisVariableList.h"
#include "AnalysisFileOutput.h"
#include "AnalysisTimer.h"
#include "AnalysisFrameData.h"
#include "AnalysisLfpData.h"
#include "AnalysisRewardData.h"
#include "AnalysisSignalData.h"
#include "AnalysisSpikeData.h"
#include "../design/PictoData.h"
#include "../memleakdetect.h"

namespace Picto {

Analysis::Analysis()
:	AssociateRoot(),
	variableFactory_(new AssetFactory(0,-1)),
	outputFactory_(new AssetFactory(0,-1)),
	dataSourceFactory_(new AssetFactory(0,-1)),
	functionFactory_(new AssetFactory(0,-1)),
	scriptFactory_(new AssetFactory(0,-1))
{
	AddDefinableObjectFactory("AnalysisVariable",variableFactory_);
	variableFactory_->addAssetType("NumberVariable",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisNumberVariable::Create))));
	variableFactory_->addAssetType("StringVariable",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisStringVariable::Create))));
	variableFactory_->addAssetType("VariableList",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVariableList::Create))));
	variableFactory_->addAssetType("VariableMap",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVariableMap::Create))));
	
	AddDefinableObjectFactory("AnalysisDataSource",outputFactory_);
	scriptFactory_->addAssetType("Timer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisTimer::Create))));
	scriptFactory_->addAssetType("Frame",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFrameData::Create))));
	scriptFactory_->addAssetType("Lfp",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisLfpData::Create))));
	scriptFactory_->addAssetType("Reward",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisRewardData::Create))));
	scriptFactory_->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisSignalData::Create))));
	scriptFactory_->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisSpikeData::Create))));


	AddDefinableObjectFactory("AnalysisOutput",dataSourceFactory_);
	dataSourceFactory_->addAssetType("File",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFileOutput::Create))));

	AddDefinableObjectFactory("AnalysisFunction",functionFactory_);
	functionFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFunction::Create))));

	AddDefinableObjectFactory("AnalysisScriptContainer",scriptFactory_);
	scriptFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisScriptContainer::Create))));
	requireUniqueChildNames(false);

	ASSOCIATE_ROOT_HOST_INITIALIZATION
}

QSharedPointer<Asset> Analysis::Create()
{
	QSharedPointer<Analysis> newAnalysis(new Analysis());
	newAnalysis->setSelfPtr(newAnalysis);
	return newAnalysis;
}

void Analysis::postDeserialize()
{
	AssociateRoot::postDeserialize();
	ASSOCIATE_ROOT_HOST_POST_DESERIALIZE
}

bool Analysis::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AssociateRoot::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
