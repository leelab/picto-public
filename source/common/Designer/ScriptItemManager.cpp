#include <QUuid>
#include "ScriptItemManager.h"
#include "../storage/DataStore.h"
#include "../parameter/AnalysisScriptContainer.h"
#include "../../common/memleakdetect.h"
ScriptItemManager::ScriptItemManager(QSharedPointer<EditorState> editorState, QGraphicsItem *scriptsParent, QSharedPointer<Asset> asset, bool horizontal)
: 
	editorState_(editorState),
	asset_(asset),
	scriptsParent_(scriptsParent),
	horizontal_(horizontal)
{
	orderedScriptNames_.append("AnalysisEntryScript");
	orderedScriptNames_.append("EntryScript");
	orderedScriptNames_.append("FrameScript");
	orderedScriptNames_.append("AnalysisFrameScript");
	orderedScriptNames_.append("ExitScript");
	orderedScriptNames_.append("AnalysisExitScript");

	scriptTypeColors_.append(QColor(Qt::yellow));
	scriptTypeColors_.append(QColor(Qt::blue));
	scriptTypeColors_.append(QColor(Qt::blue));
	scriptTypeColors_.append(QColor(Qt::yellow));
	scriptTypeColors_.append(QColor(Qt::blue));
	scriptTypeColors_.append(QColor(Qt::yellow));

	minScriptItemHeight_ = float(1.0)/(1+TYPE_NUM);

	scriptRegionStartPos_.append(0);
	scriptRegionStartPos_.append(2.5*minScriptItemHeight_);
	scriptRegionStartPos_.append(5.0*minScriptItemHeight_);
	
	updateScriptItems();
	//If the asset is edited, it may be a change in the script values so we call updateScriptItems whenever this happens
	connect(asset_.data(),SIGNAL(edited()),this,SLOT(updateScriptItems()));

	//If the asset has an analysisScriptContainer, we need to connect its edited signal too since it doesn't propegate
	//to its connected experimental asset.  This happens in performAnalysisScriptContainerOps();
	QSharedPointer<Analysis> activeAnalysis = editorState_->getCurrentAnalysis();
	if(editorState_->inAnalysisTask())
	{
		if(!asset_.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptContainer").isEmpty())
		{
			QSharedPointer<Asset> anaScriptContainer = asset_.staticCast<DataStore>()->getAssociateChildren(editorState_->getCurrentAnalysis()->getAssociateId(),"AnalysisScriptContainer").first();
			performAnalysisScriptContainerOps(anaScriptContainer);
		}
		else
		{
			//If the asset does not have an AnalysisScriptContainer, we need will connect to its childAddedAfterDeserialize() signal
			//so that performAnalysisScriptContainerOps() can operate if one is added.
			connect(asset_.data(),SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(performAnalysisScriptContainerOps(QSharedPointer<Asset>)));
		}
	}
}

void ScriptItemManager::setScriptBoundingRect(QRectF rect)
{
	scriptBoundingRect_ = rect;
	updateScriptItemDimensions();
}

void ScriptItemManager::updateScriptItemDimensions()
{
	float currPos = 0;
	float totalHeight = horizontal_?scriptBoundingRect_.width():scriptBoundingRect_.height();
	float elementHeight = 0;
	for(int i = 0; i < TYPE_NUM; i++)
	{
		if(scriptItems_.contains(i))
		{
			currPos = scriptRegionStartPos_[i/2]*totalHeight+(horizontal_?scriptBoundingRect_.topLeft().x():scriptBoundingRect_.topLeft().y());
			if( !(i%2) && scriptItems_.contains(i+1) )
			{
				elementHeight = minScriptItemHeight_*totalHeight;
			}
			else if((i%2 && scriptItems_.contains(i-1)))
			{
				elementHeight = minScriptItemHeight_*totalHeight;
				currPos += elementHeight;
			}
			else
			{
				elementHeight = minScriptItemHeight_*2.0*totalHeight;
			}
			
			ScriptItem* scriptItem = scriptItems_.value(i);
			if(horizontal_)
			{
				scriptItem->setWidth(elementHeight);
				scriptItem->setPos(QPointF(currPos,0));
				scriptItem->setHeight(scriptBoundingRect_.height());
			}
			else
			{	//Vertical
				scriptItem->setHeight(elementHeight);
				scriptItem->setPos(QPointF(0,currPos));
				scriptItem->setWidth(scriptBoundingRect_.width());
			}
		}
	}
}

void ScriptItemManager::updateScriptItems()
{
	QSharedPointer<PropertyContainer> propContainer;
	QSharedPointer<Property> prop;
	for(int i = 0; i < TYPE_NUM; i++)
	{
		bool isAnalysisScript = orderedScriptNames_[i].left(1).toLower() == "a";
		//If its a run Script
		if(!isAnalysisScript)
		{	//Set the property container directly from the asset attached to this manager
			propContainer = asset_.staticCast<DataStore>()->getPropertyContainer();
		}
		else
		{	//if its an analysis script
			//If the object does not have an AnalysisScriptContainer, skip this script
			QSharedPointer<Analysis> activeAnalysis = editorState_->getCurrentAnalysis();
			if(!editorState_->inAnalysisTask() || asset_.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptContainer").isEmpty())
				continue;

			//Set the property container from the scriptContainer
			QSharedPointer<AnalysisScriptContainer> scriptContainer = asset_.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptContainer").first().staticCast<AnalysisScriptContainer>();
			propContainer = scriptContainer.staticCast<DataStore>()->getPropertyContainer();
		}
		//Check the property container for the prop
		prop = propContainer->getProperty(orderedScriptNames_[i]);
		if(!scriptItems_.contains(i))
		{
			if(prop && !prop->value().toString().isEmpty())
			{
				scriptItems_[i] = new ScriptItem(prop,scriptTypeColors_[i],editorState_,scriptsParent_);
				scriptItems_[i]->setZValue(scriptsParent_->zValue()+1);
			}
		}
		else
		{
			Q_ASSERT(prop);
			if(prop->value().toString().isEmpty())
			{
				delete scriptItems_[i];
				scriptItems_.remove(i);
			}
		}
		
	}
	updateScriptItemDimensions();
}

void ScriptItemManager::performAnalysisScriptContainerOps(QSharedPointer<Asset> assetChild)
{
	//If the input assetChild is not an AnalysisScriptContainer, we have nothing to do stop here.
	if(!assetChild->inherits("Picto::AnalysisScriptContainer"))
		return;

	//We need to connect the AnalysisScriptContainer's edited signal to updateScriptItems() because it won't 
	//propegate to the experimental assets edited signal.
	connect(assetChild.data(),SIGNAL(edited()),this,SLOT(updateScriptItems()));
	updateScriptItems();
}