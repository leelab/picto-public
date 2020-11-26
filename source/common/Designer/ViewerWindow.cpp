#include <qscrollbar.h>
#include "ViewerWindow.h"
#include "diagramscene.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new ViewerWindow
*	\details scene is the DiagramScene that will be displayed in this Viewer.
*	editorState is used to find out when the Desiner's editMode changes (EditorState::editModeChanged), when
*	a canvas has finished loading (EditorState::windowItemsLoaded())), when zoom has changed (EditorState::zoomChanged()),
*	and to get the latest insertion item icon (EditorState::getInsertionItemPixmap()) so that the mouse can be set
*	to look like the icon that will next be inserted.
*/
ViewerWindow::ViewerWindow(QSharedPointer<EditorState> editorState, QGraphicsScene * scene, QWidget *parent) :
editorState_(editorState),
QGraphicsView(scene, parent),
scale_(1),
centerLock_(false)
{
	setRenderHint(QPainter::Antialiasing, true);
	setResizeAnchor(QGraphicsView::AnchorUnderMouse);
	setAlignment(Qt::AlignCenter);
	setDragMode(QGraphicsView::RubberBandDrag);
	connect(editorState_.data(), SIGNAL(editModeChanged(int)), this, SLOT(editModeChanged(int)));
	connect(editorState_.data(), SIGNAL(windowItemsLoaded()), this, SLOT(zoomToFitContents()));
	connect(editorState_.data(), SIGNAL(zoomChanged(double)), this, SLOT(setZoom(double)));
}

/*! \brief Extends QGraphicsView to zoom into/out of the canvas when the mouse wheel turns if the EditorState::EditMode
*	 is EditorState::Navigate.
*/
void ViewerWindow::wheelEvent(QWheelEvent *event)
{
	if (editorState_->getEditMode() != EditorState::Navigate)
	{
		event->ignore();
		QGraphicsView::wheelEvent(event);
		return;
	}

	double numDegrees = event->delta() / 8;
	double deltaScale = numDegrees / 180;

	//The code below makes the view center on the mouse after zooming
	QPoint targetViewportPos = event->pos();
	QPointF targetScenePos = mapToScene(event->pos());

	//editorState_->setZoom(currentZoom()+deltaScale);
	emit zoomChanged(currentZoom() + deltaScale);

	centerOn(targetScenePos);
	QPointF deltaViewportPos = targetViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
	QPointF viewportCenter = mapFromScene(targetScenePos) - deltaViewportPos;
	centerOn(mapToScene(viewportCenter.toPoint()));

	event->accept();
}

/*! \brief Extends the QGraphicsView::keyPressEvent() for moving the EditorState to EditorState::Navigate mode.
*	\details We use the Shift key state to transition into EditorState::Navigate mode.
*	In Navigate mode, the mouse wheel adjusts the current canvas zoom and clicking the mouse allows the designer
*	to drag the canvas position.  In EditorState::Select mode, clicking on a DiagramItem causes it to be selected.
*	\note This used to be in DiagramScene, but the implementation was frustrating as when you clicked/scrolled
*	on a diagram item with the Shift key held, it would not drag/zoom. Now, Navigate mode takes precedence over
*	all other Editor State modes.
*/
void ViewerWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift)
	{
		editorState_->setEditMode(EditorState::Navigate);
		setInteractive(false);
		event->accept();
		return;
	}
	QGraphicsView::keyPressEvent(event);
}

/*! \brief Extends the QGraphicsView::keyReleaseEvent() for moving the EditorState to EditorState::Navigate mode.
*	\details We use the Shift key state to transition to EditorState::Select mode.
*	In Navigate mode, the mouse wheel adjusts the current canvas zoom and clicking the mouse allows the designer
*	to drag the canvas position.  In EditorState::Select mode, clicking on a DiagramItem causes it to be selected.
*	\note This used to be in DiagramScene, but the implementation was frustrating as when you clicked/scrolled
*	on a diagram item with the Shift key held, it would not drag/zoom. Now, Navigate mode takes precedence over
*	all other Editor State modes.
*/
void ViewerWindow::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift)
	{
		if (editorState_->getEditMode() == EditorState::Navigate)
		{
			editorState_->setEditMode(editorState_->getLastEditMode());
			setInteractive(true);
		}
		event->accept();
		return;
	}
	QGraphicsView::keyReleaseEvent(event);
}

void ViewerWindow::resizeEvent(QResizeEvent *event)
{
	centerLock_ = true;
	zoomToFitContents();
	centerLock_ = false;
	QGraphicsView::resizeEvent(event);
}

/*! \brief Returns the current zoom level for this ViewerWindow.
*	\details Returned value is a constant that is multiplying the size of objects in the window.
*	ie. 2 means everything is twice the size.  0.5 means everything is half the size.
*/
double ViewerWindow::currentZoom()
{
	return matrix().m11();
}

/*! \brief Sets the zoom level for this viewer.
*	\details Input is a value will multiply the size of objects in the window.
*	ie. 2 means everything will be twice the size.  0.5 means everything will be half the size.
*/
void ViewerWindow::setZoom(double zoom)
{
	Q_ASSERT(zoom > 0);
	scale(zoom / matrix().m11(), zoom / matrix().m22());
}

/*! \brief Called when EditorState::EditMode changes to update the state of user interaction with the Viewer.
*	\details Essentially, this function is changing the QGraphicsViewer's DragMode and Cursor using
*	setDragMode() and setCursor() as a function of the new EditorState::EditMode.
*/
void ViewerWindow::editModeChanged(int mode)
{
	switch (mode)
	{
	case EditorState::Select:
		setDragMode(QGraphicsView::RubberBandDrag);
		setCursor(Qt::ArrowCursor);
		break;
	case EditorState::MoveItem:
		setDragMode(QGraphicsView::NoDrag);
		setCursor(Qt::SizeAllCursor);
		break;
	case EditorState::PlaceItem:
		setDragMode(QGraphicsView::NoDrag);
		setCursor(QCursor(editorState_->getInsertionItemPixmap()));
		break;
	case EditorState::Navigate:
		setDragMode(QGraphicsView::ScrollHandDrag);
		break;
	case EditorState::InsertLine:
		setDragMode(QGraphicsView::NoDrag);
		setCursor(Qt::CrossCursor);
		break;
	case EditorState::DrawLine:
		setDragMode(QGraphicsView::NoDrag);
		setCursor(Qt::CrossCursor);
		break;
	}
}

/*! \brief Zooms and moves the Viewer canvas such that the contents of the current Window Asset will all be visible.
*/
void ViewerWindow::zoomToFitContents()
{
	Q_ASSERT(this->scene());
	DiagramScene* scene = qobject_cast<DiagramScene*>(this->scene());
	fitInView(scene->getDefaultZoomRect(), Qt::KeepAspectRatioByExpanding);
	double currZoom = currentZoom();
	emit zoomChanged(currZoom);
}

/*!	\brief Reimplemented from QGraphicsView
*/
void ViewerWindow::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
	if (!scene() || rect.isNull())
		return;
	// Reset the view scale to 1:1.
	QRectF unity = matrix().mapRect(QRectF(0, 0, 1, 1));
	if (unity.isEmpty())
		return;
	scale(1 / unity.width(), 1 / unity.height());
	// Find the ideal x / y scaling ratio to fit \a rect in the view.
	int margin = 2;
	QRectF viewRect = viewport()->rect().adjusted(margin, margin, -margin, -margin);
	if (viewRect.isEmpty())
		return;
	QRectF sceneRect = matrix().mapRect(rect);
	if (sceneRect.isEmpty())
		return;
	qreal xratio = viewRect.width() / sceneRect.width();
	qreal yratio = viewRect.height() / sceneRect.height();
	// Respect the aspect ratio mode.
	switch (aspectRatioMode) {
	case Qt::KeepAspectRatio:
		xratio = yratio = qMin(xratio, yratio);
		break;
	case Qt::KeepAspectRatioByExpanding:
		xratio = yratio = qMax(xratio, yratio);
		break;
	case Qt::IgnoreAspectRatio:
		break;
	}
	// Scale and center on the center of \a rect.
	if (xratio * matrix().m11() > 1.0)
		xratio = 1.0 / matrix().m11();
	else if (xratio * matrix().m11() < 0.3)
		xratio = 0.3 / matrix().m11();

	if (yratio * matrix().m22() > 1.0)
		yratio = 1.0 / matrix().m22();
	else if (yratio * matrix().m22() < 0.3)
		yratio = 0.3 / matrix().m22();

	scale(xratio, yratio);

	if (!centerLock_)
	{
		if (viewRect.height() / sceneRect.height() < 1)
			centerOn(rect.topLeft());
		else
			centerOn(rect.center());
	}
}