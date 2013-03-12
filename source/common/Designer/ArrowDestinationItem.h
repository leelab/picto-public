#ifndef ARROWDESTIONATIONITEM_H
#define ARROWDESTIONATIONITEM_H

#include <QMap>
#include <QVector>
#include <QColor>
#include "ArrowPortItem.h"
#include "ScriptItem.h"

//! [0]
class ArrowDestinationItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowDestinationItem(QSharedPointer<EditorState> editorState, QMenu* contextMenu, QGraphicsItem *parent, QSharedPointer<Asset> asset);
	virtual ~ArrowDestinationItem(){};
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
private:
	void updateScriptItemDimensions();
	enum SCRIPTTYPE{TYPE_FIRST = 0,ANALYSIS=0,EXPERIMENTAL,TYPE_NUM};
	enum SCRIPTTIME{TIME_FIRST = 0,ENTRY=0,FRAME,EXIT,TIME_NUM};
	QVector<QString> scriptTypePrefixs_;
	QVector<QString> scriptTimeSuffixs_;
	QVector<QColor> scriptTypeColors_;
	float minScriptItemHeight_;
	QVector<float> scriptRegionStartPos_;
	QVector<QMap<int,ScriptItem*>> scriptItems_;

private slots:
	void assetEdited();
};
//! [0]

#endif
