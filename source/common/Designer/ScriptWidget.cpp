#include <QtWidgets>
#include <QVBoxLayout>
#include "ScriptWidget.h"
#include "../../common/memleakdetect.h"

//! [0]
ScriptWidget::ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, bool singleLine, QWidget *parent) :
	QWidget(parent),
	manager_(manager),
	property_(property),
	textEdit_(new ScriptTextEdit(singleLine)),
	layout_(new QVBoxLayout()),
	editorState_(editorState),
	inTextChangeDetected_(false)
{
	connect(textEdit_,SIGNAL(focusOut()),this,SIGNAL(editingFinished()));
	layout_->addWidget(textEdit_);
	layout_->setContentsMargins(QMargins(0,0,0,0));
	setLayout(layout_);
	QString text = manager_->value(property).toString();

	//Set text value to widget.
	connect(textEdit_, SIGNAL(textChanged()),this, SLOT(textChangeDetected()));
	textEdit_->setText(text);

	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
}

void ScriptWidget::textChangeDetected()
{
	//Just in case the search is considered a text change for some reason, we make this function reentrant
	if(inTextChangeDetected_)
		return;
	inTextChangeDetected_ = true;
	//Set the new text value to the underlying QtProperty.
	QString editedText = textEdit_->toPlainText();
	manager_->setValue(property_,editedText);

	//Redo search in case the new edit leads to a highlight change
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}

	//Tell the world that the text was edited
	emit textEdited(editedText);
	inTextChangeDetected_ = false;
}

void ScriptWidget::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	textEdit_->search(searchRequest,QColor(255,0,0,100));
}