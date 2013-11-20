#ifndef SyntaxErrorBox_H
#define SyntaxErrorBox_H
#include "EditorState.h"
#include "SearchableTextEdit.h"
#include "../design/DesignRoot.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

/*! \brief A Syntax Error widget used to display the output of the Picto Design Validation (ie. compilation) process.
 *	\details The Picto design is checked using DesignRoot::compiles().  If the design syntax is invalid, this
 *	can result in error messages.  These error messages are displayed in this widget.
 *	\sa DesignRoot::compiles(), Asset::validateTree()
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SyntaxErrorBox : public SearchableTextEdit
{
    Q_OBJECT

public:
   SyntaxErrorBox(QSharedPointer<EditorState> editorState, QWidget *parent=0);
   virtual ~SyntaxErrorBox(){};

   void setDesignRoot(QSharedPointer<DesignRoot> designRoot);
public slots:
	void checkSyntax();
private:
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<DesignRoot> designRoot_;

private slots:
	void searchRequested(SearchRequest searchRequest);
};
//! [0]
#endif
