#include <QtWidgets>
#include "SyntaxErrorBox.h"
#include "../../common/storage/datastore.h"
#include "../storage/SearchRequest.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
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

void SyntaxErrorBox::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	designRoot_ = designRoot;
	setText("");
}

void SyntaxErrorBox::checkSyntax()
{
	Q_ASSERT(designRoot_);
	QString errors;
	QString checkOutput;
	if(designRoot_->compiles(&errors))
	{
		checkOutput = QString("<h3 style=\"color:red\">%1</h3>").arg("Syntax Check Passed!");
	}
	else
	{
		checkOutput = QString("<h3 style=\"color:red\">%1</h3><br/>").arg("Syntax Check Failed!")+errors.toLatin1();
	}
	setText(checkOutput);
}

void SyntaxErrorBox::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	search(searchRequest,QColor(180,0,0,100));
}