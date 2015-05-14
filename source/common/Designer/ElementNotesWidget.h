#ifndef ELEMENTNOTESWIDGET_H
#define ELEMENTNOTESWIDGET_H
#include "EditorState.h"
#include "SearchableTextEdit.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

/*! \brief A widget used to enter notes that are saved with elements of the design.  This is how comments are entered in Picto Designs.
 *	\details Obviously it is extremely important to comment code.  This is how we "comment code" in the Picto Design.  Text entered into
 *	this widget is saved with the currently selected element and displayed in its tooltip.  It is also searched when a search string is
 *	typed into the Picto search box.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ElementNotesWidget : public QWidget
{
    Q_OBJECT

public:
	ElementNotesWidget(QSharedPointer<EditorState> editorState = QSharedPointer<EditorState>(), QWidget *parent=0);
	virtual ~ElementNotesWidget();

private:
	SearchableTextEdit* notesEditor_;
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<Asset> selectedAsset_;
	bool changingAsset_;
	bool inNotesWereEdited_;
	bool needsUndo_;
private slots:
	void selectedAssetChanged(QSharedPointer<Asset> asset);
	void notesWereEdited();
	void searchRequested(SearchRequest searchRequest);
	void notesLostFocus();
	void clearAssets();
};

#endif
