#include <QtWidgets>
#include <QVBoxLayout>
#include "ScriptWidget.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a new ScriptWidget
 *	\details manager and property are used to copy/set the value of the underlying QtProperty to/from the underlying
 *	ScriptTextEdit.  editorState is used to support search functionality.  Single line indicates if this ScriptWidget
 *	should contain a single line text box rather than a multi-line text editor.
 */
ScriptWidget::ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, bool singleLine, QWidget *parent) :
	QWidget(parent),
	manager_(manager),
	property_(property),
	textEdit_(new ScriptTextEdit(singleLine)),
	layout_(new QVBoxLayout()),
	editorState_(editorState),
	inTextChangeDetected_(false),
	singleLine_(singleLine)
{
	connect(textEdit_,SIGNAL(focusOut()),this,SIGNAL(editingFinished()));
	layout_->addWidget(textEdit_);
	layout_->setContentsMargins(QMargins(0, 0, 0, 0));
	setLayout(layout_);
	QString text = manager_->value(property).toString();

	//Set text value to widget.
	connect(textEdit_, SIGNAL(textChanged()),this, SLOT(textChangeDetected()));
	textEdit_->setText(text);

	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
}

/*! \brief Sets this ScriptWidget as readonly/editable depending on the bool readOnly input.*/
void ScriptWidget::setReadOnly(bool readOnly)
{
	textEdit_->setReadOnly(readOnly);
}

/*! \brief Called when the underlying ScriptTextEdit's text changes.  Updates the underlying QtProperty value accordinly
 *	and emits the textEdited() signal.
 *	\details Search is also updated in case the latest change led to a search string match appearing/disappearing.
 */
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

/*! \brief Called when a new SearchRequest is initiated in the EditorState to check the underlying ScriptTextEdit for the
 *	search string.
 */
void ScriptWidget::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	textEdit_->search(searchRequest,QColor(255,0,0,100));
}

QSize ScriptWidget::sizeHint() const
{
	if (singleLine_)
	{
		return QWidget::sizeHint();
	}

	return QSize(QWidget::sizeHint().width(), QWIDGETSIZE_MAX);
}
