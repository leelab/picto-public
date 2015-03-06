#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H
#include "EditorState.h"
#include <QSharedPointer>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

/*! \brief The ViewerWindow is the widget that displays the DiagramScene.
 *	\details In documentation, we often refer to the Designer's canvas, which is the white area
 *	on which we insert, move and select DiagramItems.  The Viewer is the widget that manages this
 *	canvas.  It scrolls it, it zooms it, it adjusts the functionality of the mouse for interacting
 *	with it, etc.  Most of the ViewerWindow functionality is supplied automatically by its parent
 *	class QGraphicsView; however, there is some customization that we need to do for Picto, which we
 *	add here.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ViewerWindow : public QGraphicsView
{
    Q_OBJECT

public:
	ViewerWindow(QSharedPointer<EditorState> editorState,QGraphicsScene * scene, QWidget *parent=0);
	virtual ~ViewerWindow(){};
protected:
	virtual void wheelEvent(QWheelEvent *event);
private:
	void validateZoom();
	double currentZoom();
	QSharedPointer<EditorState> editorState_;
	double scale_;
private slots:
	void setZoom(double zoom);
	void editModeChanged(int mode);
	void zoomToFitContents();
};

#endif
