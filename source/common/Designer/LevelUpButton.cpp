#include <QStyleOption>
#include <QPainter>
#include "LevelUpButton.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a new LevelUpButton.
 *	\details editorState is used to properly receive and handle search requests.
 */
LevelUpButton::LevelUpButton(QSharedPointer<EditorState> editorState, QWidget* parent )
	:QToolButton(parent)
{
	setIcon(QIcon(":/icons/levelup.png"));
	setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	editorState_ = editorState;
	setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 lightgrey stop: 1 grey);");
	origStyleSheet_ = styleSheet();
	connect(editorState.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
	connect(editorState.data(),SIGNAL(windowAssetChanged(QSharedPointer<Asset>)),this,SLOT(windowAssetChanged(QSharedPointer<Asset>)));
}

/*! \brief Called when a new search is requested to search the current Window Asset's ancestors for the search query and
 *	highlight the button red if the query is found or remove the highlight if it isn't.
 */
void LevelUpButton::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	QSharedPointer<Asset> windowAsset = editorState_->getWindowAsset();
	if(windowAsset.isNull())
		return;
	if(!windowAsset->inherits("Picto::DataStore"))
		return;
	if(windowAsset.staticCast<DataStore>()->searchAncestorsForQuery(searchRequest))
	{
		setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 Red stop: 0.99 darkRed);");
		show();
		highlightByGroup_[searchRequest.group] = true;
	}
	else
	{
		highlightByGroup_[searchRequest.group] = false;
		bool disableHighlight = true;
		foreach(bool highlightVal,highlightByGroup_.values())
		{
			if(highlightVal)
			{
				disableHighlight = false;
				break;
			}
		}
		if(disableHighlight)
		{
			setStyleSheet(origStyleSheet_);
			show();
		}

	}
		
}

/*! \brief Called when the Window Asset changes to redo the current search and rehighlight the button accordingly.
*/
void LevelUpButton::windowAssetChanged(QSharedPointer<Asset> windowAsset)
{
	//If any searches are already going on, trigger searchRequested now in case the window change affects the search
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}