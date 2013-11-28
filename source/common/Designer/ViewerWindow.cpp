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
	QGraphicsView(scene,parent),
	scale_(1)
{
	setRenderHint(QPainter::Antialiasing, true);
	setResizeAnchor(QGraphicsView::AnchorViewCenter);
	setAlignment(Qt::AlignCenter);
	setDragMode(QGraphicsView::RubberBandDrag);
	connect(editorState_.data(),SIGNAL(editModeChanged(int)),this,SLOT(editModeChanged(int)));
	connect(editorState_.data(),SIGNAL(windowItemsLoaded()),this,SLOT(zoomToFitContents()));
	connect(editorState_.data(),SIGNAL(zoomChanged(double)),this,SLOT(setZoom(double)));
}

/*! \brief Extends QGraphicsView to zoom into/out of the canvas when the mouse wheel turns if the EditorState::EditMode
 *	 is EditorState::Navigate.
 */
void ViewerWindow::wheelEvent(QWheelEvent *event)
{
	if(editorState_->getEditMode() != EditorState::Navigate)
	{
		event->ignore();
		QGraphicsView::wheelEvent(event);
		return;
	}
    double numDegrees = event->delta() / 8;

	double deltaScale = numDegrees/180;
	editorState_->setZoom(currentZoom()+deltaScale);
	event->accept();
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
	scale(zoom/matrix().m11(),zoom/matrix().m22());
}

/*! \brief Called when EditorState::EditMode changes to update the state of user interaction with the Viewer.
 *	\details Essentially, this function is changing the QGraphicsViewer's DragMode and Cursor using
 *	setDragMode() and setCursor() as a function of the new EditorState::EditMode.
 */
void ViewerWindow::editModeChanged(int mode)
{
	switch(mode)
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
	fitInView(scene->getDefaultZoomRect(),Qt::KeepAspectRatio);
	double currZoom = currentZoom();
	if(currZoom > 1) currZoom = 1;
	editorState_->setZoom(currZoom);
}