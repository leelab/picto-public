#ifndef SyntaxErrorBox_H
#define SyntaxErrorBox_H
#include "EditorState.h"
#include "SearchableTextEdit.h"
#include "../design/DesignRoot.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

//! [0]
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
