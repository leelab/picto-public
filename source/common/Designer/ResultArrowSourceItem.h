#ifndef ResultArrowSourceItem_H
#define ResultArrowSourceItem_H

#include "ArrowSourceItem.h"
#include "ScriptItemManager.h"

//! [0]
class ResultArrowSourceItem : public ArrowSourceItem
{
	Q_OBJECT
public:
	ResultArrowSourceItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset);
	virtual ~ResultArrowSourceItem(){};
protected:
	virtual void setRect(QRectF rect);
private:
	QSharedPointer<ScriptItemManager> scriptItemManager_;
};
//! [0]

#endif
