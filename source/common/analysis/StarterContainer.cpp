#include <QSqlQuery>
#include "StarterContainer.h"
#include "ElementTrigger.h"
#include "LFPTrigger.h"
#include "PropertyTrigger.h"
#include "SignalTrigger.h"
#include "SpikeTrigger.h"
#include "TimeTrigger.h"
#include "FrameTrigger.h"
#include "AlignTrigger.h"
#include "TransitionTrigger.h"
using namespace Picto;

StarterContainer::StarterContainer()
{
	QSharedPointer<AssetFactory> triggerFactory(new AssetFactory(0,-1));

	AddDefinableObjectFactory("Trigger",triggerFactory);
	triggerFactory->addAssetType("Element",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ElementTrigger::Create))));	
	triggerFactory->addAssetType("LFP",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LFPTrigger::Create))));	
	triggerFactory->addAssetType("Property",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PropertyTrigger::Create))));
	triggerFactory->addAssetType("Frame",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FrameTrigger::Create))));
	triggerFactory->addAssetType("Align",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AlignTrigger::Create))));
	triggerFactory->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalTrigger::Create))));	
	triggerFactory->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SpikeTrigger::Create))));	
	triggerFactory->addAssetType("Time",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimeTrigger::Create))));	
	triggerFactory->addAssetType("Transition",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TransitionTrigger::Create))));	

}

StarterContainer::~StarterContainer()
{

}

QSharedPointer<Asset> StarterContainer::Create()
{
	return QSharedPointer<Asset>(new StarterContainer());
}

QList<QSharedPointer<Asset>> StarterContainer::getTriggers()
{
	return getGeneratedChildren("Trigger");
}

EventOrderIndex StarterContainer::getNextTriggerInList(EventOrderIndex afterIndex)
{
	EventOrderIndex returnVal;
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QSharedPointer<AnalysisTrigger> trigger;
	EventOrderIndex currIndex;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		currIndex = trigger->getCurrentTrigger();
		while(currIndex <= afterIndex)
		{
			currIndex = trigger->getNextTrigger();
			if(!currIndex.isValid())
				break;
		}
		if(currIndex.isValid())
		{
			if(!returnVal.isValid() || currIndex < returnVal)
			{
				returnVal = currIndex;
			}
		}
	}
	return returnVal;
}


void StarterContainer::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool StarterContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	if(!triggers.size())
	{
		addError("At least one Trigger must be defined in an StarterContainer.");
		return false;
	}
	if(triggers.size() > 1)
	{
		foreach(QSharedPointer<Asset> trigger,triggers)
		{
			if(trigger.staticCast<AnalysisTrigger>()->getDataSource() != EventOrderIndex::BEHAVIORAL)
			{
				addError("Multiple start triggers are only allowed for Behavioral Triggers.");
				return false;
			}
		}
	}
	return true;
}