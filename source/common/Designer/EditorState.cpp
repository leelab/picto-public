#include "EditorState.h"
#include <QtWidgets>
#include <QSharedPointer>
#include <QInputDialog>
#include "../../common/storage/DataStore.h"
#include "../../common/statemachine/uienabled.h"
#include "AssetItem.h"
#include "ArrowDestinationItem.h"
#include "StartBarItem.h"
#include "../../common/memleakdetect.h"

EditorState::EditorState()
	:	editMode_(Select),
		lastEditMode_(Select),
		lineColor_(Qt::black),
		insertItemCategory_(Select),
		insertItemType_(""),
		windowAssetPath_(""),
		selectedItem_(NULL)
{

}

/*! \brief Returns a list of the current SearchRequest objects.  All searchable elements in the Designer should be
 *	serching for these.
 *	\sa SearchRequest
 */
QList<SearchRequest> EditorState::getSearchRequests()
{
	return searchRequests_.values();
}
/*! \brief Sets the top level asset for this Designer.
 *	\details The top level asset is the Asset whose children will be visible when the Designer won't let us navigate any
 *	further up the design tree.  Currently, this is set by the Designer to be the Experiment Asset.
 */
void EditorState::setTopLevelAsset(QSharedPointer<Asset> topLevelAsset)
{
	topAsset_ = topLevelAsset;
	Q_ASSERT(topAsset_);
	setCurrentAnalysis(QSharedPointer<Analysis>());
	//setWindowAsset(topAsset_);
}

/*! \brief Sets the input Analysis as the currently active analysis in the Designer.
 *	\details When there is an active Analysis, things are displayed differently on the Designer canvas.
 *	AnalysisElements specific to the current Analysis are shown.  Experimental elements are grayed out.
 *	Only AnalysisElements are shown in the Toolbox.  Since Analyses can exist on their own and be attached to different
 *	experiments, this function checks to see if the input Analysis is already linked to the Experiment being designed.
 *	If it is not linked to the Experiment, the function links it.  Any incompatibilities or other problems in the link are
 *	reported to the user in a message box.  Then the Analysis is set to active in
 *	DesignConfig::setActiveAnalysisIds() and the currentAnalysisChanged() signal is emitted.
 */
bool EditorState::setCurrentAnalysis(QSharedPointer<Analysis> currAnalysis)
{
	if (!topAsset_)
	{
		return false;
	}
	//Run through this function even if the current analysis hasn't changed, because
	//the current analysis in the designConfig may have been changed and this will 
	//reset it.

	//If there is a current analysis
	if(currAnalysis)
	{
		//Verify that its linked into the experiment.  
		QSharedPointer<Experiment> exp = topAsset_.staticCast<Experiment>();
		Q_ASSERT(exp);		
		//Link the analysis to the experiment
		QString feedback;
		bool linkResult = currAnalysis->LinkToAsset(exp,feedback);
		if(!linkResult)
		{	//If anything didn't go perfectly in the link (had to link by paths or failed) tell the user.
			QMessageBox msg;
			msg.setText("The Analysis was not fully compatible with this Experiment.");
			msg.setDetailedText(feedback);
			msg.setIcon(QMessageBox::Warning);
			msg.exec();
		}
	}
	currAnalysis_ = currAnalysis;
	QList<QUuid> activeAnalysisList;
	if(currAnalysis_)
		activeAnalysisList.append(currAnalysis_->getAssociateId());
	topAsset_->getDesignConfig()->setActiveAnalysisIds(activeAnalysisList);
	emit currentAnalysisChanged(currAnalysis_);
	return true;
}

/*! \brief Sets the current EditorState::EditMode to the input EditorState::EditMode.
 *	\details When called, this function emits the editModeChanged() signal with the new EditorState::EditMode.
 */
void EditorState::setEditMode(int mode)
{
	lastEditMode_ = editMode_;
	editMode_ = EditMode(mode);
	emit editModeChanged(mode);
}
/*! \brief Call this to set the zoom level of the canvas to the input value.
 *	\details The input zoom will be limited to the range (.3,2).  When called, this function emits zoomChanged() with the
 *	new zoom value.  The zoom value is essentially a constant by which the size of each object will be multiplied.
 *	\note Don't confuse this zoom's functionality with the zoom used in the TestViewer and RemoteViewer windows to zoom
 *	in and out of a running experiment.  That zoom is defined differently from this one.
 */
double EditorState::setZoom(double zoom)
{
	if(zoom < .3)
		zoom = .3;
	if(zoom > 2)
		zoom = 2;
	emit zoomChanged(zoom);
	return zoom;
}

/*! \brief Sets the current insertion item that will be added to the  Window Asset if the user clicks in the canvas.
 *	\details className is the name of the Asset's class, friendlyName is the friendlyName for the Asset's class that is
 *	shown in the UI.  Category and type are the name of the XML tag and its type property value used to store this Asset
 *	within the WindowAsset's XML tag.  pixmap is the Asset's icon which can replace the mouse icon to show the designer
 *	what type of Asset they are about to create before they click.  This function emits the insertionItemChanged() signal.
 *	\note When this function is called, the EditorState::EditMode is changed from EditorState::Select to
 *	EditorState::PlaceItem or the opposite depending if the input value is an insertable item or empty data
 *	(indicating that there is no current insertable item).
 */
void EditorState::setInsertionItem(QString className, QString friendlyName, QString category, QString type, QPixmap pixmap)
{
	insertItemCategory_ = category;
	insertItemType_ = type;
	insertItemPixmap_ = pixmap;
	if(!category.isEmpty() || !type.isEmpty())
		setEditMode(EditorState::PlaceItem);
	else if(getEditMode() == EditorState::PlaceItem)
		setEditMode(EditorState::Select);
	emit insertionItemChanged(className,friendlyName);
}

/*! \brief Sets the current asset opened in the editor window to the input asset object
 *	\details If undoable is true, an undo point will be set after changing the window asset (so that undo after a window
 *	change will bring you back to the previous window and undo after a window change and other action will keep you in the
 *	same window).
 *
 *	This function will emit the windowAssetChanged() signal if the input Asset is a valid Window Asset.
 *	\sa Designer::setOpenAsset()
 */
void EditorState::setWindowAsset(QSharedPointer<Asset> asset,bool undoable)
{
	if (!asset)
	{
		return;
	}
	Q_ASSERT(!asset.objectCast<DataStore>().isNull());
	Q_ASSERT(!asset.objectCast<UIEnabled>().isNull());
	QSharedPointer<UIEnabled> uiEnabled = asset.objectCast<UIEnabled>();
	//If this asset has no editable descendants, it should not be openable.
	if(!uiEnabled->hasEditableDescendants())
	{
		return;
	}

	//Store the current window asset
	windowAsset_ = asset;

	//Clear any selected items to insert into the scene
	setInsertionItem();

	//Set the currently selected item to null
	setSelectedItem(NULL);
	
	//Tell everyone that the window asset changed.
	emit windowAssetChanged(windowAsset_);

	//Set the window asset as the currently selected asset.
	setSelectedAsset(asset);

	if(undoable)
	{
		//Set this action as undoable so that when people undo the next change they make, they
		//will still be on the window where they made that change, rather than moving back to 
		//the last place where they made an undoable change.
		setLastActionUndoable();
	}
}

/*! \brief Sets the Window Asset to the parent of the current Window Asset.
 */
void EditorState::setWindowAssetToParent()
{
	if(!getWindowAsset().isNull() 
		&& !getWindowAsset()->getParentAsset().isNull() 
		&& !getWindowAsset()->getParentAsset().dynamicCast<UIEnabled>().isNull())
		setWindowAsset(getWindowAsset()->getParentAsset());
}

/*! \brief Sets the Designer's currently selected Asset to the input object.
 *	\details This function emits selectedAssetChanged()
 */
void EditorState::setSelectedAsset(QSharedPointer<Asset> asset)
{
	if(selectedAsset_ == asset)
		return;
	selectedAsset_ = asset;
	emit selectedAssetChanged(selectedAsset_);
}

/*! \brief Sets the Designer's currently selected DiagramItem or Arrow to the input object.
 *	\details Internally, this function checks the input item's type and calls setSelectedAsset() with an appropriate input.
 *	If the input item is an AssetItem or a sub-graphic, it calls setSelectedAsset() with the referenced Asset.
 *	If it is anything else, it calls setSelectedAsset() with the current Window Asset.  This function emits the
 *	itemSelected() or arrowPortSelected() signals as appropriate.
 *	\sa setSelectedAsset()
 */
void EditorState::setSelectedItem(QGraphicsItem *item)
{
	AssetItem* assetItem = dynamic_cast<AssetItem*>(item);
	ArrowPortItem* arrowPortItem = dynamic_cast<ArrowPortItem*>(item);

	selectedItem_ = item;
	if(item && item->isSelected())
	{
		if(assetItem)
		{
			setSelectedAsset(assetItem->getAsset());
		}
		else if(arrowPortItem)
		{
			setSelectedAsset(arrowPortItem->getAsset());
		}
		else
		{
			setSelectedAsset(getWindowAsset());
		}
		if(arrowPortItem)
			emit arrowPortSelected(arrowPortItem->getAsset());
	}
	else
	{
		if(		!item 
			|| (assetItem && (assetItem->getAsset() == selectedAsset_)) )
			setSelectedAsset(getWindowAsset());
	}
	emit itemSelected(selectedItem_);
}

/*! \brief Call this function to cause the current Designer window to be reloaded.
 *	\details This function clears the insertion item, then emits the windowAssetChanged() signal with the current Window
 *	Asset and then emits selectedAssetChanged() with the current selected Asset.  This has the effect of telling all
 *	widgets connected to these signals to reload themselves.
 *	\note This function does not appear to be in use right now.  If you decide to use it, it is probably worth testing to
 *	make sure it does the job correctly.
 */
void EditorState::reloadWindow()
{
	Q_ASSERT(getWindowAsset());
	Q_ASSERT(getSelectedAsset());
	setInsertionItem();
	emit windowAssetChanged(getWindowAsset());
	emit selectedAssetChanged(getSelectedAsset());
}

/*! \brief Call this function to request a search for the input searchRequest.
 *	\details When called, the searchRequested() signal is emitted.  All searchable elements in the Designer should respond
 *	to that signal by searching for all active searchRequests returned from getSearchRequests().
 *	\sa SearchRequest
 */
void EditorState::requestSearch(SearchRequest searchRequest)
{
	if(searchRequest.type == SearchRequest::INVALID_TYPE || searchRequest.group==SearchRequest::INVALID_GROUP)
		return;
	if(searchRequest.type == SearchRequest::STRING && searchRequest.query == "")
		searchRequest.enabled = false;

	searchRequests_[searchRequest.getGroupTypeIndex()] = searchRequest;
	emit searchRequested(searchRequest);
}

/*!	This function is called to clear out the shared pointers that are preventing the experiment from being deleted.
 *	The EditorState will have to be reinitialized before it can be used, meaning this should just be called when leaving
 *	the Designer.
 */
void EditorState::deinitEditor()
{
	selectedAsset_ = QSharedPointer<Asset>();
	windowAsset_ = QSharedPointer<Asset>();
	topAsset_ = QSharedPointer<Asset>();
	currAnalysis_ = QSharedPointer<Analysis>();

	//emit selectedAssetChanged(selectedAsset_);

	emit releaseEditorMemory();
}
