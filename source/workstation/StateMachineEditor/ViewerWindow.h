#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H
#include "EditorState.h"
#include <QSharedPointer>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

//! [0]
class ViewerWindow : public QGraphicsView
{
    Q_OBJECT

public:
	ViewerWindow(QSharedPointer<EditorState> editorState,QGraphicsScene * scene, QWidget *parent=0);
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
//! [0]
#endif
