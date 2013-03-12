#include <QtWidgets>
#include <QVBoxLayout>
#include "ScriptWidget.h"
#include "../../common/memleakdetect.h"

//! [0]
ScriptWidget::ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QWidget *parent) :
	QWidget(parent),
	manager_(manager),
	property_(property),
	textEdit_(new ScriptTextEdit()),
	layout_(new QVBoxLayout())
{
	connect(textEdit_,SIGNAL(focusOut()),this,SIGNAL(editingFinished()));
	layout_->addWidget(textEdit_);
	setLayout(layout_);
	QString text = manager_->value(property).toString();

	//Set text value to widget.
	textEdit_->setText(text);
	connect(textEdit_, SIGNAL(textChanged()),this, SLOT(setScriptValue()));
	layout_->setContentsMargins(QMargins(0,0,0,0));
}

void ScriptWidget::setScriptValue()
{
	QString editedText = textEdit_->toPlainText();

	//Set fixed text value to qtproperty.
	manager_->setValue(property_,editedText);
	//Tell the world that the text was edited
	emit textEdited(editedText);
}