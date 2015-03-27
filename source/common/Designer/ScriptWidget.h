#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QtVariantProperty.h>
#include "ScriptTextEdit.h"
#include "EditorState.h"

/*! \brief A widget for managing and presenting ScriptTextEdit widgets as well as interfacing them with an underlying
 *	QtProperty.
 *	\details This widget is used with the PropertyEditorFactory to create a widget for updating Script Properties.  As
 *	such, this widget needs to include interfacing with the QtPropertBrowser framework which we use to get some free
 *	automatic type dependant widget creation and layout.  Changes to the widget text are set to an underlyiung QtProperty
 *	using its QtVariantPropertyManager.  Similarly, changes to the underlying QtProperty are reflected in the ScriptWidget
 *	text.  The ScriptWidget can include a single line text box or a full fledged multi-line text editor.  Either way, due
 *	to this widgets use of an underlying ScriptTextEdit, the script text is syntax highlighted and searchable.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */	
class ScriptWidget : public QWidget
{
    Q_OBJECT

public:
   ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, bool singleLine = false, QWidget *parent=0);
   virtual ~ScriptWidget(){};
   void setReadOnly(bool readOnly);
   QSize sizeHint() const;
signals:
   /*! \brief Emitted whenever the ScriptWidget's text is changed. text is the widget's new text*/
   void textEdited(const QString & text);
   /*! \brief Emitted whenever the text edit looses focus.  This is a better signal than textEdited() to connect to when it comes to defining undo blocks so
    *	that not every letter is considered for undoing.*/
   void editingFinished();
private:
	QToolButton* createButton();
	QLayout *layout_;
	ScriptTextEdit* textEdit_;
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	QSharedPointer<EditorState> editorState_;
	int lineStartTabs_;
	bool inTextChangeDetected_;
	bool singleLine_;
private slots:
	void textChangeDetected();
	void searchRequested(SearchRequest searchRequest);
};


#endif
