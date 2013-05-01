#include "ViewerWindow.h"
#include "diagramscene.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
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

double ViewerWindow::currentZoom()
{
	return matrix().m11();
}

void ViewerWindow::setZoom(double zoom)
{
	Q_ASSERT(zoom > 0);
	scale(zoom/matrix().m11(),zoom/matrix().m22());
}

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

void ViewerWindow::zoomToFitContents()
{
	Q_ASSERT(this->scene());
	DiagramScene* scene = qobject_cast<DiagramScene*>(this->scene());
	fitInView(scene->getDefaultZoomRect(),Qt::KeepAspectRatio);
	double currZoom = currentZoom();
	if(currZoom > 1) currZoom = 1;
	editorState_->setZoom(currZoom);
}