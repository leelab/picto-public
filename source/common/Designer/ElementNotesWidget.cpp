#include <QtWidgets>
#include <QMetaObject>
#include <QLabel>
#include "ElementNotesWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new ElementNotesWidget.
 *	\details The EditorState needs to be passed in so that this widget knows when the selected Asset changes and
 *	what the latest SearchRequest is.  The notes entered into this widget are also saved with the Design, so the
 *	widget uses the EditorState to report EditorState::undoableActionPerformed().
 */
ElementNotesWidget::ElementNotesWidget(QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	editorState_(editorState),
	changingAsset_(false),
	inNotesWereEdited_(false),
	notesEditor_(new SearchableTextEdit())
{
	setLayout(new QVBoxLayout());
	layout()->setContentsMargins(QMargins(0,0,0,0));
	layout()->addWidget(new QLabel("Notes:"));
	layout()->addWidget(notesEditor_);

	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),this,SLOT(selectedAssetChanged(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(releaseEditorMemory()), this, SLOT(clearAssets()));
	connect(notesEditor_, SIGNAL(textChanged()),this, SLOT(notesWereEdited()));
	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));

	//Set up undoable action trigger
	connect(notesEditor_,SIGNAL(focusOut()),this,SLOT(notesLostFocus()));
}

ElementNotesWidget::~ElementNotesWidget()
{
}

/*! \brief Called when the Designer's selected Asset changes to update the contents of this widget
 *	with that Asset's notes.
 */
void ElementNotesWidget::selectedAssetChanged(QSharedPointer<Asset> asset)
{
	if(asset && !asset->inherits("Picto::UIEnabled"))
		return;
	changingAsset_ = true;
	selectedAsset_ = asset;
	if (selectedAsset_.isNull())
	{
		notesEditor_->setText("");
	}
	else
	{
		notesEditor_->setText(asset.staticCast<UIEnabled>()->getNotes());
	}
	changingAsset_ = false;
}

/*! \brief Called when the text in the underlying SearchableTextEdit changes to updated the stored notes in the 
 *	currently selected Asset and redo the current search.
 */
void ElementNotesWidget::notesWereEdited()
{
	//Don't do anything if the reason for the change was that we're changing the selected asset
	if(changingAsset_ || selectedAsset_.isNull())
		return;

	//Just in case the search is considered a text change for some reason, we make this function reentrant
	if(inNotesWereEdited_)
		return;
	inNotesWereEdited_ = true;
	//Set the new notes to the asset
	Q_ASSERT(selectedAsset_->inherits("Picto::UIEnabled"));
	selectedAsset_.staticCast<UIEnabled>()->setNotes(notesEditor_->toPlainText());

	//Redo search in case the new edit leads to a highlight change
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
	
	//Since something happened, the system will need an undo.
	needsUndo_ = true;
	inNotesWereEdited_ = false;
}

/*! \brief Performs the input SearchRequest on the underlying SearchableTextEdit
 */
void ElementNotesWidget::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	notesEditor_->search(searchRequest,QColor(255,0,0,100));
}

/*! \brief Called when the underlying SearchableTextEdit looses focus to tell the EditorState that
 *	 an undoable action was performed.
 *	\details It seemed like a good idea to undo actions in the ElementNotesWidget in blocks between 
 *	getting and losing focus rather than making every single letter change undoable.  Due to Picto's
 *	currently inefficient undo/redo system, making every letter undoable always takes a very long time as well.
 *	\sa EditorState::undoableActionPerformed(), Designer::insertEditBlock
 */
void ElementNotesWidget::notesLostFocus()
{
	if(!needsUndo_)
		return;
	needsUndo_ = false;
	editorState_->undoableActionPerformed();
}

//! Called to release Designer assets so it may be cleaned up.  Will need to be reinitialized before use.
void ElementNotesWidget::clearAssets()
{
	selectedAsset_ = QSharedPointer<Asset>();
}