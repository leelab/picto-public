#include <QtWidgets>
#include "AssetInfoBox.h"
#include "../../common/storage/datastore.h"
#include "../storage/SearchRequest.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs an AssetInfoBox with the input EditorState object and parent widget.
 *	\details The constructor connects signals from the EditorState to this widget to let
 *	it know when the selected Asset has changed and when a new search was requested.
 *	\sa EditorState::selectedAssetChanged(), EditorState::searchRequested()
 */
AssetInfoBox::AssetInfoBox(QSharedPointer<EditorState> editorState,QWidget *parent) :
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
/*! \brief Updates the text in this AssetInfoBox to reflect the currently selected Asset.
 *	\details Retrieves contextual information on the Asset from the Asset::getInfo() function.
 */
void AssetInfoBox::assetSelected(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	setText(asset->getInfo());
	
	//Search for whatever search is currently active when the AssetInfoBox contents change
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}
/*! \brief Called whenever the requested search changes to update highlighted text in the Text box.*/
void AssetInfoBox::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	search(searchRequest,QColor(180,0,0,100));
}