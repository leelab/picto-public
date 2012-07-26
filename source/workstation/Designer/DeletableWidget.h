#ifndef DELETABLEWIDGET_H
#define DELETABLEWIDGET_H
#include "EditorState.h"
#include <QSharedPointer>
#include <QWidget>
#include <QtVariantPropertyManager>

//! [0]
class DeletableWidget : public QWidget
{
    Q_OBJECT

public:
	DeletableWidget(QSharedPointer<EditorState> editorState, QtVariantPropertyManager* manager, QtProperty* property, QWidget* child, QWidget *parent=0);
	virtual ~DeletableWidget(){};
private:
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	QSharedPointer<EditorState> editorState_;
	QWidget *child_;
private slots:
	void triggered(int);
};
//! [0]
#endif
