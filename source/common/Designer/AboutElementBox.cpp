#include <QtWidgets>
#include "AboutElementBox.h"
#include "../../common/storage/datastore.h"
#include "../storage/SearchRequest.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AboutElementBox::AboutElementBox(QSharedPointer<EditorState> editorState,QWidget *parent) :
	SearchableTextEdit(parent),
	editorState_(editorState)
{
	setLineWrapMode(NoWrap);
	setTabStopWidth(16);
	setReadOnly(true);
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));

	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
}

void AboutElementBox::assetSelected(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	setText(asset->getDescription());
	
	//Search for whatever search is currently active when the AboutElementBox contents change
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}

void AboutElementBox::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	search(searchRequest,QColor(180,0,0,100));
}