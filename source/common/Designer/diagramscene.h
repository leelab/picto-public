#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "DiagramItemFactory.h"
#include "EditorState.h"
#include "Copier.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
#include <QPointer>
using namespace Picto;

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE
 /*! \brief The QGraphicsScene shown in the Designer's central widget to display the children of the current element of
  *	the Design's State Machine.
  *	\details We sometimes refer to this element as a canvas and it is the surface on which all of the DiagramItems and
  *	arrows are drawn to represent the current level of the StateMachine.  The Scene handles things like creating and
  *	inserting new DiagramItems for new Assets that are added to the current Window Asset.  It handles copying, deleting,
  *	and selecting of elements, navigation and zooming, adding of new assets and transitions to the window element, etc.
  *	It is the foundation on which all of the graphical StateMachine UI is based.
  *	\note There are some functions here that we don't use.  This is due to the fact that much of the code for this and
  *	other elements of the graphical StateMachine UI were lifted from Qt Toolkit samples.
  *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
  *	\date 2009-2015
  */
class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:

    DiagramScene(QSharedPointer<EditorState> editorState, QMenu *itemContextMenu, QMenu *sceneContextMenu, QObject *parent = 0);
	virtual ~DiagramScene(){};

	QGraphicsLineItem* insertTransition(DiagramItem* source, DiagramItem* dest, QSharedPointer<Asset> transition = QSharedPointer<Asset>());
	DiagramItem* insertDiagramItem(QSharedPointer<Asset> asset,QPointF pos);
	QRectF getDefaultZoomRect();


public slots:
	void setSceneAsset(QSharedPointer<Asset> asset);
	void setLineColor(const QColor &color);
    void editorLostFocus(DiagramTextItem *item);
	void deleteSelectedItems();
	void copySelectedItems();
	void analysisImportSelectedItem();
	void analysisExportSelectedItem();
	void pasteItems();

private slots:
	void clearAssets();

signals:
	/*! \brief Emitted every time that a DiagramItem is created and inserted into the DiagramScene.
	 *	@param item is the newly created DiagramItem.
	 */
    void itemInserted(DiagramItem *item);
	/*! \brief Emitted whenever a DiagramItem or Arrow is selected.
	 *	@param item is a QGraphicsItem pointer to the selected Item.
	 */
    void itemSelected(QGraphicsItem *item);
	/*! \brief Emitted whenever an Asset is selected.
	 *	@param asset is a QGraphicsItem pointer to the selected Item.
	 */
	void assetSelected(QSharedPointer<Asset> asset);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
	QSharedPointer<Asset> createNewAsset();
    bool isItemChange(int type);
	QList<QSharedPointer<Asset>> getSelectedAssets();

	QSharedPointer<EditorState> editorState_;
	QSharedPointer<DiagramItemFactory> diagItemFactory_;
	QSharedPointer<Copier> copier_;
	DiagramItem* startBar_;
	QString insertionItem_;
	int newItemIndex_;
    QMenu *myItemMenu;
	QMenu *sceneMenu_;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    DiagramTextItem *textItem;
	QPoint latestPastePos_;
	bool useNavigateMode_;
	bool mouseOverScene_;
};


#endif
