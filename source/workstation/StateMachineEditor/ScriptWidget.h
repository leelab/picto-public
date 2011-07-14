#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H
#include <QTextEdit>
#include <QtVariantPropertyManager>

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
private:
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	int lineStartTabs_;
private slots:
	void setScriptValue();
};
//! [0]
#endif