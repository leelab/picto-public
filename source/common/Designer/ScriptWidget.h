#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QtVariantProperty.h>
#include "ScriptTextEdit.h"

//! [0]
//Widget for managing and presenting ScriptTextEdit's as well as its interface with an underlying QtProperty
//collapsible indicates that the script widget will be collapsed to its name only unless it has contents.
//If it does have contents, it will be maximized and won't be minimizable until those contents are deleted
//or whitespace only remains
class ScriptWidget : public QWidget
{
    Q_OBJECT

public:
   ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QWidget *parent=0);
   virtual ~ScriptWidget(){};
signals:
   void textEdited(const QString &);
   void editingFinished();
private:
	QToolButton* createButton();
	QLayout* layout_;
	ScriptTextEdit* textEdit_;
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	int lineStartTabs_;
	bool collapsible_;
	QToolButton* collapseButton_;
private slots:
	void setScriptValue();
};

//! [0]
#endif
