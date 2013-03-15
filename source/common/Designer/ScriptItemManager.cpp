#include "ScriptItemManager.h"
#include "../storage/DataStore.h"
#include "../../common/memleakdetect.h"
ScriptItemManager::ScriptItemManager(QSharedPointer<EditorState> editorState, QGraphicsItem *scriptsParent, QSharedPointer<Asset> asset)
: 
	editorState_(editorState),
	asset_(asset),
	scriptsParent_(scriptsParent)
{
	scriptTypePrefixs_.append("Analysis");
	scriptTypePrefixs_.append("");
	scriptTimeSuffixs_.append("EntryScript");
	scriptTimeSuffixs_.append("FrameScript");
	scriptTimeSuffixs_.append("ExitScript");

	scriptTypeColors_.append(QColor(Qt::yellow));
	scriptTypeColors_.append(QColor(Qt::blue));

	minScriptItemHeight_ = float(1.0)/(1+TIME_NUM*TYPE_NUM);

	scriptRegionStartPos_.append(0);
	scriptRegionStartPos_.append((TYPE_NUM+0.5)*minScriptItemHeight_);
	scriptRegionStartPos_.append((2*TYPE_NUM+1)*minScriptItemHeight_);
	
	for(int stm = TIME_FIRST; stm < TIME_NUM; stm++)
	{
		scriptItems_.append(QMap<int,ScriptItem*>());
	}

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
	float totalHeight = scriptBoundingRect_.height();
	float elementHeight = 0;
	for(int stm = TIME_FIRST; stm < TIME_NUM; stm++)
	{
		currPos = scriptRegionStartPos_[stm]*totalHeight+scriptBoundingRect_.topLeft().y();
		foreach(ScriptItem* scriptItem,scriptItems_[stm])
		{
			elementHeight = minScriptItemHeight_*totalHeight*(TYPE_NUM/scriptItems_[stm].size());
			scriptItem->setHeight(elementHeight);
			scriptItem->setPos(QPointF(0,currPos));
			currPos += elementHeight;
			scriptItem->setWidth(scriptBoundingRect_.width());
		}
	}
}

void ScriptItemManager::assetEdited()
{
	QSharedPointer<PropertyContainer> propContainer_ = asset_.staticCast<DataStore>()->getPropertyContainer();
	QSharedPointer<Property> prop;
	for(int stm = TIME_FIRST; stm < TIME_NUM; stm++)
	{
		for(int stp = TYPE_FIRST; stp < TYPE_NUM; stp++)
		{
			prop = propContainer_->getProperty(scriptTypePrefixs_[stp]+scriptTimeSuffixs_[stm]);
			if(!scriptItems_[stm].contains(stp))
			{
				if(prop && !prop->value().toString().isEmpty())
				{
					scriptItems_[stm][stp] = new ScriptItem(prop,scriptTypeColors_[stp],editorState_,scriptsParent_);
					scriptItems_[stm][stp]->setZValue(scriptsParent_->zValue()+1);
				}
			}
			else
			{
				Q_ASSERT(prop);
				if(prop->value().toString().isEmpty())
				{
					delete scriptItems_[stm][stp];
					scriptItems_[stm].remove(stp);
				}
			}
		}
	}
	updateScriptItemDimensions();
}