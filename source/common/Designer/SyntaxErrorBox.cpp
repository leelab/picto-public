#include <QtWidgets>
#include "SyntaxErrorBox.h"
#include "../../common/storage/datastore.h"
#include "../storage/SearchRequest.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a SyntaxErrorBox with the input EditorState object and parent widget.
 *	\details The constructor connects to editorState's EditorState::searchRequested() to 
 *	get messages about when the requested search changes.
 */
SyntaxErrorBox::SyntaxErrorBox(QSharedPointer<EditorState> editorState,QWidget *parent) :
	SearchableTextEdit(parent),
	editorState_(editorState)
{
	setLineWrapMode(NoWrap);
	setTabStopWidth(16);
	setReadOnly(true);

	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
}

/*! \brief Sets the current DesignRoot of the Picto Design to this widget so that it can be used to check syntax.
 */
void SyntaxErrorBox::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	designRoot_ = designRoot;
	setText("");
}

/*! \brief Checks the syntax of the Picto Design rooted at the DesignRoot input in setDesignRoot().  Outputs results in this Widgets textual display.
 *	\details This function uses DesignRoot::compiles() to check the syntax.
 */
void SyntaxErrorBox::checkSyntax()
{
	Q_ASSERT(designRoot_);
	QString errors;
	QString checkOutput;
	if(designRoot_->compiles(&errors))
	{
		checkOutput = QString("<h3 style=\"color:blue\">%1</h3>").arg("Syntax Check Passed!");
	}
	else
	{
		checkOutput = QString("<h3 style=\"color:red\">%1</h3><br/>").arg("Syntax Check Failed!")+errors.toLatin1();
	}
	setText(checkOutput);
}
/*! \brief Called whenever the requested search changes to update highlighted text in the Text box.*/
void SyntaxErrorBox::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	search(searchRequest,QColor(180,0,0,100));
}