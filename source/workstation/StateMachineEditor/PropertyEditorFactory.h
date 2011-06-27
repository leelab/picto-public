#ifndef PROPERTYEDITORFACTORY_H
#define PROPERTYEDITORFACTORY_H
#include <QtVariantEditorFactory>
#include <QSharedPointer>
#include <QtVariantPropertyManager>
#include "EditorState.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyEditorFactory : public QtVariantEditorFactory
{
    Q_OBJECT

public:
   PropertyEditorFactory(QSharedPointer<EditorState> editorState,QWidget *parent=0);
signals:
	void propertyChanged();

protected:
	virtual QWidget* createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent);
	virtual bool eventFilter(QObject *obj, QEvent *event);
private:
	QSharedPointer<EditorState> editorState_;
	bool childPropWasChanged_;
};
//! [0]
#endif
