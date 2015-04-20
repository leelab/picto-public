#include <QUuid>
#include "ScriptItemManager.h"
#include "../storage/DataStore.h"
#include "../parameter/AnalysisScriptHolder.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new ScriptItemManager.
 *	\details editorState is used to extract information about any current Analysis and in ScriptItem constructors.
 *	scriptsParanet is the DiagramItem that will be the parent of the scripts created by this object.
 *	asset is used to figure out which ScriptItems need to be created and to connect to various signals telling us 
 *	if we need to change something about the ScriptItems in response to a change in the Asset.
 *	horizontal lets us know if the ScriptItems will be positioned one after the other in a horizontal or vertical direction.
 */
ScriptItemManager::ScriptItemManager(QSharedPointer<EditorState> editorState, QGraphicsItem *scriptsParent, QSharedPointer<Asset> asset, bool horizontal)
: 
	editorState_(editorState),
	asset_(asset),
	scriptsParent_(scriptsParent),
	horizontal_(horizontal)
{
	orderedScriptNames_.append("AnalysisEntryScript");
	orderedScriptNames_.append("OperatorEntryScript");
	orderedScriptNames_.append("EntryScript");
	orderedScriptNames_.append("FrameScript");
	orderedScriptNames_.append("AnalysisFrameScript");
	orderedScriptNames_.append("OperatorFrameScript");
	orderedScriptNames_.append("ExitScript");
	orderedScriptNames_.append("OperatorExitScript");
	orderedScriptNames_.append("AnalysisExitScript");

	scriptTypeColors_.append(QColor(Qt::yellow));
	scriptTypeColors_.append(QColor(Qt::gray));
	scriptTypeColors_.append(QColor(Qt::blue));
	scriptTypeColors_.append(QColor(Qt::blue));
	scriptTypeColors_.append(QColor(Qt::yellow));
	scriptTypeColors_.append(QColor(Qt::gray));
	scriptTypeColors_.append(QColor(Qt::blue));
	scriptTypeColors_.append(QColor(Qt::gray));
	scriptTypeColors_.append(QColor(Qt::yellow));

	minScriptItemHeight_ = float(1.0)/(1+TYPE_NUM);

	scriptRegionStartPos_.append(0);
	scriptRegionStartPos_.append(2.5*minScriptItemHeight_);
	scriptRegionStartPos_.append(5.0*minScriptItemHeight_);
	
	updateScriptItems();
	//If the asset is edited, it may be a change in the script values so we call updateScriptItems whenever this happens
	connect(asset_.data(),SIGNAL(edited()),this,SLOT(updateScriptItems()));

	//If the asset has an AnalysisScriptHolder, we need to connect its edited signal too since it doesn't propegate
	//to its connected experimental asset.  This happens in performAnalysisScriptHolderOps();
	QSharedPointer<Analysis> activeAnalysis = editorState_->getCurrentAnalysis();
	if(activeAnalysis)
	{
		if(!asset_.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptHolder").isEmpty())
		{
			QSharedPointer<Asset> anaScriptContainer = asset_.staticCast<DataStore>()->getAssociateChildren(editorState_->getCurrentAnalysis()->getAssociateId(),"AnalysisScriptHolder").first();
			performAnalysisScriptHolderOps(anaScriptContainer);
		}
		else
		{
			//If the asset does not have an AnalysisScriptHolder, we need will connect to its childAddedAfterDeserialize() signal
			//so that performAnalysisScriptHolderOps() can operate if one is added.
			connect(asset_.data(),SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(performAnalysisScriptHolderOps(QSharedPointer<Asset>)));
		}
	}
}

/*! \brief Sets the bounding rectangle in which ScriptItems will be layed out.
 *	\details Generally this is going to just be the shape of the parent DiagramItem.
 */
void ScriptItemManager::setScriptBoundingRect(QRectF rect)
{
	scriptBoundingRect_ = rect;
	updateScriptItemDimensions();
}

/*! \brief Updates the positions and sizes of all ScriptItems managed by this object.
 *	\details This is called if something changes that requires the ScriptItems to be redrawn.
 *	(ie. scriptBoundingRect changes, a ScriptItems are added/deleted).
 */
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

/*! \brief Creates and lays out all ScriptItems.
 *	\details This is called whenever something changes that causes ScriptItem objects to be added or removed.
 *	This happens, for example, when we switch the Designer mode from Experimental design to Analysis design.
 *	When that happens there are suddenly extra analysis scripts that need to appear within StartBars.  Also,
 *	when a Script's text is emptied, corresponding ScriptItem objects are removed.
 *	Layout is handled by calling updateScriptItemDimensions().
 */
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
			//If the object does not have an AnalysisScriptHolder, skip this script
			QSharedPointer<Analysis> activeAnalysis = editorState_->getCurrentAnalysis();
			if(!activeAnalysis || asset_.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptHolder").isEmpty())
				continue;

			//Set the property container from the scriptContainer
			QSharedPointer<AnalysisScriptHolder> scriptContainer = asset_.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptHolder").first().staticCast<AnalysisScriptHolder>();
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

/*! \brief Sets up a connection between newly added AnalysisScriptHolder objects and this manager so that changes in those elements will be tracked.
 *	\details AnalysisScriptHolders' edited signals don't propegate up to the experimental assets to which they are connected.  This means that
 *	if their contents are edited, this object won't find out about it unless we make sure to connect the AnalysisScriptHolder's edited() signal
 *	itself to updateScriptItems().  That is what we do here.
 */
void ScriptItemManager::performAnalysisScriptHolderOps(QSharedPointer<Asset> assetChild)
{
	//If the input assetChild is not an AnalysisScriptHolder, we have nothing to do stop here.
	if(!assetChild->inherits("Picto::AnalysisScriptHolder"))
		return;

	//We need to connect the AnalysisScriptHolder's edited signal to updateScriptItems() because it won't 
	//propegate to the experimental assets edited signal.
	connect(assetChild.data(),SIGNAL(edited()),this,SLOT(updateScriptItems()));
	updateScriptItems();
}