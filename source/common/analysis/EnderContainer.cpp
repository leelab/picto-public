#include <QSqlQuery>
#include "EnderContainer.h"
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

EnderContainer::EnderContainer()
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

EnderContainer::~EnderContainer()
{

}

QSharedPointer<Asset> EnderContainer::Create()
{
	return QSharedPointer<Asset>(new EnderContainer());
}

QList<QSharedPointer<Asset>> EnderContainer::getTriggers()
{
	return getGeneratedChildren("Trigger");
}

EventOrderIndex EnderContainer::getNextTriggerInList(EventOrderIndex afterIndex)
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

void EnderContainer::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool EnderContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	if(!triggers.size())
	{
		addError("At least one Trigger must be defined in an EnderContainer.");
		return false;
	}
	return true;
}