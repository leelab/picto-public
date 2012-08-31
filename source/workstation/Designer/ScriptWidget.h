#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H
#include <QTextEdit>
#include <QtVariantPropertyManager>
#include "ScriptSyntaxHighlighter.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class ScriptWidget : public QTextEdit
{
    Q_OBJECT

public:
   ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QWidget *parent=0);
   virtual ~ScriptWidget(){};
   virtual bool event(QEvent* e);
signals:
   void editingFinishedAndTextEdited();
protected:
	virtual void focusOutEvent(QFocusEvent *e);
private:
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	int lineStartTabs_;
	bool textEdited_;
	ScriptSyntaxHighlighter *syntaxHighlighter_;
private slots:
	void setScriptValue();
};
































//! [0]
#endif
