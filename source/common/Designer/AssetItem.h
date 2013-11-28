#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <QGraphicsSvgItem>
#include "DiagramItem.h"
#include "../../common/storage/asset.h"

#include "EditorState.h"
using namespace Picto;

/*! \brief A DiagramItem used to display DataStores.... it should probably have been called DataStoreItem.
 *	\details Represents a Diagram DataStore with a name, type and icon image.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class AssetItem : public DiagramItem
{
	Q_OBJECT
public:
	AssetItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~AssetItem();
	/*! \brief Returns the Asset that this DiagramItem represents.*/
	QSharedPointer<Asset> getAsset(){return asset_;};
	friend class DiagramItemFactory;

public slots:
	void assetEdited();
protected:
	virtual void updateDependantGraphics(){};
	virtual void positionChanged(QPoint pos);
	virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	void setSvgIcon(QGraphicsSvgItem* svgIcon);
	void setSvgIcon(QString svgFile);
	void keepPixmapAspectRatio(bool keep);
	virtual void setRect(QRectF rect);
	QGraphicsSvgItem* getSvgItem();
private:
	QSharedPointer<Asset> asset_;
	QGraphicsSvgItem* lastSvgIcon_;
	QString svgFileName_;
	bool posChanged_;
	bool posInitialized_;
	bool keepAspectRatio_;

private slots:
	void searchRequested(SearchRequest searchRequest);

};


#endif
