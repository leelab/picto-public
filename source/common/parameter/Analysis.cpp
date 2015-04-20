#include "Analysis.h"
#include "AnalysisDataSource.h"
#include "AnalysisFunction.h"
#include "AnalysisScriptHolder.h"
#include "AnalysisNumberVariable.h"
#include "AnalysisStringVariable.h"
#include "AnalysisVariableMap.h"
#include "AnalysisVariableList.h"
#include "AnalysisFileOutput.h"
#include "AnalysisTimer.h"
#include "AnalysisFrameData.h"
#include "AnalysisLfpData.h"
#include "AnalysisRewardData.h"
#include "AnalysisRunNotesData.h"
#include "AnalysisSignalData.h"
#include "AnalysisSpikeData.h"
#include "../storage/ObsoleteNameAsset.h"
#include "../design/PictoData.h"

#include "../operator/BarPlot.h"
#include "../operator/HistogramPlot.h"
#include "../operator/SamplingBarPlot.h"
#include "../operator/SamplingHistogramPlot.h"


#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new Analysis object.
 *	\details Sets up the various AssetFactories for creation of all of the Analysis elements.
 */
Analysis::Analysis()
:	AssociateRoot(),
	variableFactory_(new AssetFactory(0,-1)),
	outputFactory_(new AssetFactory(0,-1)),
	dataSourceFactory_(new AssetFactory(0,-1)),
	functionFactory_(new AssetFactory(0,-1)),
	scriptFactory_(new AssetFactory(0,-1)),
	dataViewElementFactory_(new AssetFactory(0, -1))
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
	
	AddDefinableObjectFactory("AnalysisDataSource",dataSourceFactory_);
	dataSourceFactory_->addAssetType("Timer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisTimer::Create))));
	dataSourceFactory_->addAssetType("Frame",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFrameData::Create))));
	dataSourceFactory_->addAssetType("Lfp",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisLfpData::Create))));
	dataSourceFactory_->addAssetType("Reward",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisRewardData::Create))));
	dataSourceFactory_->addAssetType("RunNotes",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisRunNotesData::Create))));
	dataSourceFactory_->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisSignalData::Create))));
	dataSourceFactory_->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisSpikeData::Create))));


	AddDefinableObjectFactory("AnalysisOutput",outputFactory_);
	outputFactory_->addAssetType("File",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFileOutput::Create))));

	AddDefinableObjectFactory("AnalysisFunction",functionFactory_);
	functionFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFunction::Create))));

	AddDefinableObjectFactory("AnalysisScriptHolder",scriptFactory_);
	scriptFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisScriptHolder::Create))));
	requireUniqueChildNames(false);

	AddDefinableObjectFactory("DataViewElement", dataViewElementFactory_);
	dataViewElementFactory_->addAssetType(BarPlot::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(BarPlot::Create))));
	dataViewElementFactory_->addAssetType(SamplingBarPlot::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(SamplingBarPlot::Create))));
	dataViewElementFactory_->addAssetType(HistogramPlot::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(HistogramPlot::Create))));
	dataViewElementFactory_->addAssetType(SamplingHistogramPlot::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(SamplingHistogramPlot::Create))));

	//A copy paste error led to all data sources being saved with the AnalysisScriptContainer tag before Picto version 1.0.30 and syntax version 0.0.2
	//For this reason we changed the AnalysisScriptContainer name to AnalysisScriptHolder and in the upgradeVersion function we are recreating all 
	//AnalysisDataSource and AnalysisScriptHolder objects with the correct tag names.
	AddDefinableObjectFactory("AnalysisScriptContainer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteNameAsset::Create))));

	ASSOCIATE_ROOT_HOST_INITIALIZATION
}

/*! \brief Creates a new Analysis object and returns a shared asset pointer to it.*/
QSharedPointer<Asset> Analysis::Create()
{
	QSharedPointer<Analysis> newAnalysis(new Analysis());
	newAnalysis->setSelfPtr(newAnalysis);
	return newAnalysis;
}

/*! \brief Extends AssociateRoot::upgradeVersion() to fix a specific bug in Analysis serializations from before design syntax version 0.0.2
 *	\details Before 0.0.2, a copy paste error led to all data sources being saved with the AnalysisScriptContainer tag.  See function code for more details.
 */
void Analysis::upgradeVersion(QString deserializedVersion)
{
	AssociateRoot::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.2")
	{	
		//Before 0.0.2, a copy paste error led to all data sources being saved with the AnalysisScriptContainer tag 
		//For this reason we changed the AnalysisScriptContainer name to AnalysisScriptHolder and in the upgradeVersion function we are recreating all 
		//AnalysisDataSource and AnalysisScriptHolder objects with the correct tag names.
		QList<QSharedPointer<Asset>> childList = getGeneratedChildren("AnalysisScriptContainer");
		
		//Go through everything that was saved with an AnalysisScriptContainer tag.
		QSharedPointer<ObsoleteNameAsset> obsNameChild;
		foreach(QSharedPointer<Asset> child,childList)
		{
			obsNameChild = child.staticCast<ObsoleteNameAsset>();
			
			//If the type was empty, change the name to AnalysisScriptHolder
			if(obsNameChild->getOriginalType().isEmpty())
				obsNameChild->setNewTagName("AnalysisScriptHolder");
			else	//If there is a type, change the name to AnalysisDataSource
				obsNameChild->setNewTagName("AnalysisDataSource");
		}
	}
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
