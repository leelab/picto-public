#ifndef ELEMENTNOTESWIDGET_H
#define ELEMENTNOTESWIDGET_H
#include "EditorState.h"
#include "SearchableTextEdit.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
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
};
//! [0]
#endif
