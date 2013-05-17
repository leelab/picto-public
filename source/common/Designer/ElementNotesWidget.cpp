#include <QtWidgets>
#include <QMetaObject>
#include <QLabel>
#include "ElementNotesWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
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
	connect(notesEditor_, SIGNAL(textChanged()),this, SLOT(notesWereEdited()));
	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));

	//Set up undoable action trigger
	connect(notesEditor_,SIGNAL(focusOut()),this,SLOT(notesLostFocus()));
}

ElementNotesWidget::~ElementNotesWidget()
{
}

void ElementNotesWidget::selectedAssetChanged(QSharedPointer<Asset> asset)
{
	if(!asset->inherits("Picto::UIEnabled"))
		return;
	changingAsset_ = true;
	selectedAsset_ = asset;
	if(selectedAsset_.isNull())
		notesEditor_->setText("");
	else
		notesEditor_->setPlainText(asset.staticCast<UIEnabled>()->getNotes());
	changingAsset_ = false;
}

void ElementNotesWidget::notesWereEdited()
{
	//Don't do anything if the reason for the change was that we're chaging the selected asset
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

void ElementNotesWidget::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	notesEditor_->search(searchRequest,QColor(255,0,0,100));
}

void ElementNotesWidget::notesLostFocus()
{
	if(!needsUndo_)
		return;
	needsUndo_ = false;
	editorState_->undoableActionPerformed();
}