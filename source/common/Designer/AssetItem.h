#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <QGraphicsSvgItem>
#include "DiagramItem.h"
#include "../../common/storage/asset.h"

#include "EditorState.h"
using namespace Picto;

//! [0]
class AssetItem : public DiagramItem
{
	Q_OBJECT
public:
	AssetItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~AssetItem();
	QSharedPointer<Asset> getAsset(){return asset_;};
	friend class DiagramItemFactory;

public slots:
	void assetEdited();
protected:
	virtual void updateDependantGraphics(){};
	virtual void positionChanged(QPoint pos);
	virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
	void setSvgIcon(QGraphicsSvgItem* svgIcon);
	void setSvgIcon(QString svgFile);
	void keepPixmapAspectRatio(bool keep);
	virtual void setRect(QRectF rect);
private:
	QSharedPointer<Asset> asset_;
	QGraphicsSvgItem* svgItem_;
	bool posChanged_;
	bool posInitialized_;
	bool keepAspectRatio_;

};
//! [0]

#endif
