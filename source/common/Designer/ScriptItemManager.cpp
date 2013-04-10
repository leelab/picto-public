#include "ScriptItemManager.h"
#include "../storage/DataStore.h"
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
	scriptRegionStartPos_.append(5*minScriptItemHeight_);
	
	assetEdited();
	connect(asset_.data(),SIGNAL(edited()),this,SLOT(assetEdited()));
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
			if( (i%2 && scriptItems_.contains(i-1)) || (!(i%2) && scriptItems_.contains(i+1)) )
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
			currPos += elementHeight;
		}
	}
}

void ScriptItemManager::assetEdited()
{
	QSharedPointer<PropertyContainer> propContainer_ = asset_.staticCast<DataStore>()->getPropertyContainer();
	QSharedPointer<Property> prop;
	for(int i = 0; i < TYPE_NUM; i++)
	{
		
			prop = propContainer_->getProperty(orderedScriptNames_[i]);
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