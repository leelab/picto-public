#include "Analysis.h"
#include "AnalysisDataSource.h"
#include "AnalysisFunction.h"
#include "AnalysisScriptContainer.h"
#include "AnalysisVar.h"
#include "AnalysisVarHash.h"
#include "AnalysisVarList.h"
#include "AnalysisOutput.h"
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
	variableFactory_->addAssetType("Var",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVar::Create))));
	variableFactory_->addAssetType("VarList",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVarList::Create))));
	variableFactory_->addAssetType("VarHash",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVarHash::Create))));
	
	AddDefinableObjectFactory("AnalysisDataSource",outputFactory_);
	//dataSourceFactory_->addAssetType("Spike",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisSpikeSource::Create))));

	AddDefinableObjectFactory("AnalysisOutput",dataSourceFactory_);
	//outputFactory_->addAssetType("File",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFileOutput::Create))));

	AddDefinableObjectFactory("AnalysisFunction",functionFactory_);
	functionFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFunction::Create))));

	AddDefinableObjectFactory("AnalysisScriptContainer",scriptFactory_);
	scriptFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisScriptContainer::Create))));

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
