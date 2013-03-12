#include <QGraphicsScene>
#include "ArrowDestinationItem.h"
#include "ArrowSourceItem.h"
#include "Arrow.h"
#include "../storage/DataStore.h"
#include "../../common/memleakdetect.h"
ArrowDestinationItem::ArrowDestinationItem(QSharedPointer<EditorState> editorState, QMenu* contextMenu, QGraphicsItem *parent, QSharedPointer<Asset> asset) :
ArrowPortItem("",editorState,contextMenu,parent,asset)
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
	connect(asset.data(),SIGNAL(edited()),this,SLOT(assetEdited()));
}

QPointF ArrowDestinationItem::getContactPoint()
{
	return getRect().center()-QPointF(getRect().width()/2.0,0);
}

void ArrowDestinationItem::setRect(QRectF rect)
{
	ArrowPortItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,QColor(0,210,0));
	grad.setColorAt(1,QColor(0,130,0));
	QBrush brush(grad);
	setBrush(brush);

	updateScriptItemDimensions();
}

void ArrowDestinationItem::updateScriptItemDimensions()
{
	float currPos = 0;
	float totalHeight = getHeight();
	float elementHeight = 0;
	for(int stm = TIME_FIRST; stm < TIME_NUM; stm++)
	{
		currPos = scriptRegionStartPos_[stm]*totalHeight;
		foreach(ScriptItem* scriptItem,scriptItems_[stm])
		{
			elementHeight = minScriptItemHeight_*totalHeight*(TYPE_NUM/scriptItems_[stm].size());
			scriptItem->setHeight(elementHeight);
			scriptItem->setPos(QPointF(0,currPos));
			currPos += elementHeight;
			scriptItem->setWidth(getWidth());
		}
	}
}

void ArrowDestinationItem::assetEdited()
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
					scriptItems_[stm][stp] = new ScriptItem(prop,scriptTypeColors_[stp],getEditorState(),this);
					scriptItems_[stm][stp]->setZValue(zValue()+1);
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