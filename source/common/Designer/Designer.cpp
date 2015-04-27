#include <QtWidgets>
#include <QLabel>
#include <QMenuBar>
#include <QSizePolicy>

#include "toolbox.h"
#include "Designer.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "AssetItem.h"
#include "arrow.h"
#include "PropertyBrowser.h"
#include "AssetInfoBox.h"
#include "ViewerWindow.h"
#include "ElementNotesWidget.h"
#include "../parameter/AssociateElement.h"
#include "../storage/SearchRequest.h"
#include "UIHelper.h"
#include "../../common/memleakdetect.h"

const int InsertTextButton = 10;

/*! \brief Constructs a new Designer widget with the input parent.
 *	\details This function initializes the UI and all variables.  It also creates the EditorState object used throughout
 *	the designer to maintain the current state and allow different components to communicate with one another.
 */
Designer::Designer(QWidget *parent) :
	QWidget(parent),
	editorState_(new EditorState())
{
	scene = NULL;
	view = NULL;
	createActions();
	createMenus();
    createToolbars();
	topmostScene = new DiagramScene(editorState_,itemMenu,sceneMenu);
	loadScene(topmostScene);
	connectActions();
	
	QVBoxLayout *mainLayout = new QVBoxLayout;

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(editToolBar);
    toolbarLayout->addWidget(pointerToolbar);
	toolbarLayout->addStretch();

	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	QSplitter *centralLayout = new QSplitter(Qt::Vertical);
	upButton = new LevelUpButton(editorState_);
	connect(upButton,SIGNAL(pressed()),editorState_.data(),SLOT(setWindowAssetToParent()));

	//Setup info pane
	infoPane_ = new QTabWidget();
	aboutElementBox_ = new AboutElementBox(editorState_);
	infoPane_->addTab(aboutElementBox_,"About Element");
	assetInfoBox_ = new AssetInfoBox(editorState_);
	infoPane_->addTab(assetInfoBox_,"Scripting Info");
	syntaxErrorBox_ = new SyntaxErrorBox(editorState_);
	infoPane_->addTab(syntaxErrorBox_,"Error List");

	infoPane_->setTabEnabled(ABOUTELEMENT,true);
	infoPane_->setTabEnabled(SCRIPTINFO,true);
	infoPane_->setTabEnabled(SYNTAXCHECK,false);

    centralLayout->addWidget(upButton);
	centralLayout->setStretchFactor(0,0);
	centralLayout->addWidget(view);
	centralLayout->setStretchFactor(1,10);
	centralLayout->addWidget(infoPane_);
	centralLayout->setStretchFactor(2,7);

	centralLayout->handle(1)->setDisabled(true);
	UIHelper::addSplitterLine(centralLayout->handle(2), 150);
	centralLayout->setHandleWidth(11);

	toolbox_ = new Toolbox(editorState_);
	splitter->addWidget(toolbox_);
    splitter->addWidget(centralLayout);

	QSplitter* rightSideWidget = new QSplitter(Qt::Vertical);
	propertyEditor_ = new PropertyBrowser(editorState_);
	notesWidget_ = new ElementNotesWidget(editorState_);
	rightSideWidget->addWidget(propertyEditor_);
	rightSideWidget->addWidget(notesWidget_);

	UIHelper::addSplitterLine(rightSideWidget->handle(1), 50);
	rightSideWidget->setHandleWidth(11);

	QList<int> rightSideSizes;
	rightSideSizes << 500 << 100;
	rightSideWidget->setSizes(rightSideSizes);

	splitter->addWidget(rightSideWidget);
	splitter->setStretchFactor(1,10);

	UIHelper::addSplitterLine(splitter->handle(1), 150);
	UIHelper::addSplitterLine(splitter->handle(2), 150);
	splitter->setHandleWidth(11);

	mainLayout->addLayout(toolbarLayout);
	mainLayout->addWidget(splitter);

	setLayout(mainLayout);
}

Designer::~Designer()
{
}

/*! \brief Called before displaying the Designer on screen to set the DesignRoot on which it will operate.
 *	\details In essence, a Designer's entire purpose is to operate on a DesignRoot.  Call this function
 *	to initialize it for operation with the input DesignRoot.
 */
void Designer::loadDesign(QSharedPointer<DesignRoot> designRoot)
{
	Q_ASSERT(designRoot);
	if(designRoot_ != designRoot)
	{
		if(designRoot_)
		{
			disconnect(designRoot_.data(),SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
			disconnect(designRoot_.data(),SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));
		}
		designRoot_ = designRoot;
		//design_ = designRoot->getDesign(identifier,index);
		Q_ASSERT(designRoot_);
		connect(designRoot_.data(),SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
		connect(designRoot_.data(),SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));

		//Tell the syntaxErrorBox about the designRoot
		syntaxErrorBox_->setDesignRoot(designRoot_);

		//Populate the AnalysisSelector
		editorState_->setCurrentAnalysis(QSharedPointer<Analysis>());
		analysisOption_->setDesignRoot(designRoot_);

		//Since we just loaded a new design, there are no undos or redos available
		undoAvailable(false);
		redoAvailable(false);
	}
	resetEditor();
}

/*! \brief Use this function instead of setEnabled() to enable/disable the Designer.
 *	\details This function not only enable/disables user interaction with the Designer like setEnabled() does,
 *	it also disables some of the internal operations of the Designer such that editing of the Design from
 *	other locations will not have undesirable side effects.
 */
void Designer::activate(bool enable)
{
	if(!enable)
	{
		//If we keep an asset selected, this viewer can have unexpected effects even though it is not active.
		//For example, an unexpected path leading from property value changes through the property browser back to
		//a different part of the propery could be active if we didn't do this
		editorState_->setSelectedAsset(QSharedPointer<Asset>());
	}
	setEnabled(enable);
}

/*! \brief Called when the zoom combo box is updated to change the zoom level of the designer canvas.*/
void Designer::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
	editorState_->setZoom(newScale);
}

/*! \brief This is an initialization function that loads the input scene as the current Designer Scene.
 *	\details Originally, the designer used to function differently with lots of different scenes being loaded back and
 *	forth depending on which Element was the Window Asset.  Now we only use a single scene that updates itself depending
 *	on the Window Asset.  This functions contents could probably just be moved into a larger intialization function at
 *	some point.
 */
void Designer::loadScene(DiagramScene* newScene)
{
	if(scene == newScene)
		return;

	//Switch Scene
	scene = newScene;

	//Load new Scene
	newScene->setSceneRect(QRectF(0, 0, 4000, 4000));
	if(view == NULL)
		view = new ViewerWindow(editorState_,newScene);
	view->setScene(newScene);
	//In this version of Qt it looks like connecting signals to slots more than once does not have the effect of
	//  multiplying the number of times that the slots are called.  In future versions this may occur though because
	//  future versions support a Qt::UniqueConnection parameter.  We would just disconnect old connections before
	//  connecting the new ones but this isn't working for some reason.
	connect(editorState_.data(),SIGNAL(resetExperiment()),this,SLOT(resetExperiment()));
}

/*! \brief Resets the associated DesignRoot from its XML code and then resets the Designer to function with the newly
 *	reset object.
 *	\details This is useful because some operations currently require rebuildling the design from text.  For example,
 *	currently deleting an element in Picto just sets a deleted flag on that element that tells it not to serialize itself
 *	out.  This helps with maintaining the general serialized XML structure through changes to the design, but in order to
 *	actually make the element disappear in the Designer we need to then rebuild the whole design from its XML text.
 *	Obviously that is extremely inefficient but this is the kind of tradeoff that sometimes sped up the Picto development
 *	schedule at the expense of runtime efficiency.  If you have time, please rework the system such that we won't need to
 *	use inefficient functions like this one.
 */
void Designer::resetExperiment()
{
	//Make sure the latest changes are included in the XML text.
	insertEditBlock();
	//Rebuild the design root from the latest XML text.
	designRoot_->refreshFromXml();
	//Reset the editor to work with the new objects in the DesignRoot.
	resetEditor();
}
/*! \brief Puts the latest changes to the Design into the serialized XML that we use for undo/redo operations.
 *	\details We make undo/redo slow but easy to program by serializing the Design every time an important change
 *	was made and copying it into a document who's operations are tracked for undo/redo purposes.  That way to undo/redo
 *	we just undo/redo the document and reload the Design from that document.
 */
void Designer::insertEditBlock()
{
	designRoot_->setUndoPoint();
}

/*! \brief Undoes the last significant action in the Design.
 *	\details Calls undo on the underlying XML document, rebuilds the designRoot based on that document and resets
 *	this Designer to work with the new objects underlying the DesignRoot.
 *	\sa insertEditBlock()
 */
void Designer::performUndoAction()
{
	if(!designRoot_->hasUndo())
		return;
	designRoot_->undo();
	resetEditor();
}
/*! \brief Redoes a previously undone significant action in the Design.
 *	\details Calls redo on the underlying XML document, rebuilds the designRoot based on that document and resets
 *	this Designer to work with the new objects underlying the DesignRoot.
 *	\sa insertEditBlock()
 */
void Designer::performRedoAction()
{
	if(!designRoot_->hasRedo())
		return;
	designRoot_->redo();
	resetEditor();
}

/*! \brief Tells the DesignRoot which asset is currently open in the Designer so that this information can be saved out
 *	with the Design.
 *	\details Along with the other UI specific information like Elements' canvas positions, we save the currently opened
 *	asset in the Design file.  This allows the Designer to open Designs to the last Window Asset that was open when they
 *	were saved.  It also helps in Undo/Redo by moving us to the spot where we were before the last edit block occured
 *	(for undo) or after the next redo block occured (for redo).
 */
void Designer::setOpenAsset(QSharedPointer<Asset> asset)
{
	Q_ASSERT(asset);
	designRoot_->setOpenAsset(asset);
}

/*! \brief Called when the selected Asset in the Designer canvas changes to update the actions available in the
 *	Toolbar/ContextMenu accordingly.
 *	\note This would appear to be redundant with selectedItemChanged(), it might be worth looking into.
 */
void Designer::selectedAssetChanged(QSharedPointer<Asset>)
{
	updateEnabledActions();
}

/*! \brief Called when the selected Graphics Item in the Designer canvas changes to update the actions available in the
 *	Toolbar/ContextMenu accordingly.
 *	\note This would appear to be redundant with selectedAssetChanged(), it might be worth looking into.
 */
void Designer::selectedItemChanged(QGraphicsItem*)
{
	updateEnabledActions();
}

/*! \brief Called when the current Analysis changes to reload the window asset.
 *	\details If we are switching Analyses or into or out of an Analysis mode, things are displayed differently on the
 *	Designer canvas.  AnalysisElement specific to the current Analysis are shown.  If an Analysis is active, experimental
 *	elements need to be grayed out.  This assures that the editorState redraws its window properly now that it knows
 *	about the currently selected analysis.
 *	\note This appears to just be helping the EditorState do its job.  Probably this functionality should be moved out of
 *	this class and just happen automatically inside the EditorState when it emits the EditorState::currentAnalysisChanged()
  *	signal.
 */
void Designer::currentAnalysisChanged(QSharedPointer<Analysis>)
{
	//When current analysis changes, reload the window
	QSharedPointer<Asset> openAsset = designRoot_->getOpenAsset();
	if(!openAsset)
		openAsset = editorState_->getTopLevelAsset();
	editorState_->setWindowAsset(openAsset,false);
}

/*! Checks the syntax of the current XML to see if it is a legal experiment
 *	\details In the process it sets the displayed tab in the InfoPane to the SyntaxErrorBox tab.
 *	so that the designer can see the output.
*/
void Designer::checkSyntax()
{
	QString errors;
	infoPane_->setTabEnabled(SYNTAXCHECK,true);
	infoPane_->setCurrentIndex(SYNTAXCHECK);
	syntaxErrorBox_->checkSyntax();
}

/*! \brief Creats actions used in various parts of the Designer (ie. Toolbar, ContextMenu, etc).
*/
void Designer::createActions()
{
	undoAction = new QAction(QIcon(""),
                                tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setStatusTip(tr("Undo last action"));

	redoAction = new QAction(QIcon(""),
                                tr("&Redo"), this);
    redoAction->setShortcut(tr("Ctrl+Y"));
    redoAction->setStatusTip(tr("Redo action"));

    deleteAction = new QAction(QIcon(":/icons/delete.png"),
                               tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));

	copyAction = new QAction(QIcon(":/icons/copy.png"),
                               tr("Copy"), this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip(tr("Copy elements from your design"));

	analysisExportAction = new QAction(QIcon(":/icons/export_analysis.png"),
                               tr("Export Analysis Elements"), this);
    analysisExportAction->setStatusTip(tr("Export analysis elements from an experimental asset"));

	analysisImportAction = new QAction(QIcon(":/icons/import_analysis.png"),
                               tr("Import Analysis Elements"), this);
    analysisImportAction->setStatusTip(tr("Import analysis elements to an experimental asset"));

	pasteAction = new QAction(QIcon(":/icons/paste.png"),
								tr("Paste"), this);
	pasteAction->setShortcut(tr("Ctrl+V"));
	pasteAction->setStatusTip(tr("Paste copied elements"));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    exitAction->setStatusTip(tr("Quit Scenediagram example"));

	searchBox = new QLineEdit("");
	searchBox->setPlaceholderText(tr("Enter Search String"));
	connect(searchBox,SIGNAL(textChanged(QString)),this,SLOT(searchTextChanged(QString)));
	matchCase = new QCheckBox("Match case");
	connect(matchCase,SIGNAL(stateChanged(int)),this,SLOT(matchCaseChanged(int)));
	QVBoxLayout* searchLayout = new QVBoxLayout();
	searchLayout->addWidget(searchBox);
	searchLayout->addWidget(matchCase);
	searchWidget = new QFrame();
	searchWidget->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken); 
	searchWidget->setLayout(searchLayout);

	analysisOption_ = new AnalysisOptionWidget(editorState_);
	//analysisOption_->setVisible(false);//REMOVE THIS LINE TO USE ANALYSIS

	//Turn on highlighting for elements with children that have scripts and analysis scripts.
	editorState_->requestSearch(SearchRequest(SearchRequest::EXPERIMENT,SearchRequest::SCRIPT));
	editorState_->requestSearch(SearchRequest(SearchRequest::ACTIVE_ANALYSES,SearchRequest::SCRIPT));

	checkSyntaxAction_ = new QAction(tr("&Check XML syntax"),this);
	checkSyntaxAction_->setShortcut(Qt::Key_F7);
	checkSyntaxAction_->setToolTip(tr("Check the current experiment's XML code for syntax errors."));
	checkSyntaxAction_->setIcon(QIcon(":/icons/checksyntax.png"));
	connect(checkSyntaxAction_, SIGNAL(triggered()), this, SLOT(checkSyntax()));
}

/*! \brief Connects actions created in createActions() to the functionality that should occur when they are triggered.
*/
void Designer::connectActions()
{
	connect(undoAction, SIGNAL(triggered()),this, SLOT(performUndoAction()));
	connect(redoAction, SIGNAL(triggered()),this, SLOT(performRedoAction()));
	connect(editorState_.data(),SIGNAL(undoRequested()),this,SLOT(performUndoAction()));

    connect(deleteAction, SIGNAL(triggered()),scene, SLOT(deleteSelectedItems()));
	connect(copyAction, SIGNAL(triggered()),scene, SLOT(copySelectedItems()));
	connect(analysisExportAction, SIGNAL(triggered()),scene, SLOT(analysisExportSelectedItem()));
	connect(analysisImportAction, SIGNAL(triggered()),scene, SLOT(analysisImportSelectedItem()));
	connect(pasteAction, SIGNAL(triggered()),scene, SLOT(pasteItems()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(editorState_.data(),SIGNAL(undoableActionPerformed()),this,SLOT(insertEditBlock()));
	connect(editorState_.data(),SIGNAL(windowAssetChanged(QSharedPointer<Asset>)),this,SLOT(setOpenAsset(QSharedPointer<Asset>)));
	connect(editorState_.data(),SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),this,SLOT(selectedAssetChanged(QSharedPointer<Asset>)));
	connect(editorState_.data(),SIGNAL(itemSelected(QGraphicsItem*)),this,SLOT(selectedItemChanged(QGraphicsItem*)));
	connect(editorState_.data(),SIGNAL(currentAnalysisChanged(QSharedPointer<Analysis>)),this,SLOT(currentAnalysisChanged(QSharedPointer<Analysis>)));
}

/*! \brief Creates context menus using actions created in createActions().
*/
void Designer::createMenus()
{
	itemMenu = new QMenu(tr("&Item"),this);
    itemMenu->addAction(deleteAction);
	itemMenu->addAction(copyAction);
	itemMenu->addAction(pasteAction);
	itemMenu->addAction(analysisExportAction);
	itemMenu->addAction(analysisImportAction);

	sceneMenu = new QMenu(tr("Scene"),this);
	sceneMenu->addAction(pasteAction);
}

/*! \brief Creates the Designer's Toolbars using actions created in createActions() as well as other widgets
 *	created here.
 */
void Designer::createToolbars()
{

    editToolBar = new QToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
	editToolBar->addAction(copyAction);
	editToolBar->addAction(pasteAction);
	editToolBar->addAction(analysisExportAction);
	editToolBar->addAction(analysisImportAction);
	editToolBar->addAction(undoAction);
	editToolBar->addAction(redoAction);

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    pointerToolbar = new QToolBar(tr("Pointer type"));

    pointerToolbar->addWidget(sceneScaleCombo);
	pointerToolbar->addSeparator();
	pointerToolbar->addWidget(searchWidget);
	pointerToolbar->addSeparator();
	pointerToolbar->addAction(checkSyntaxAction_);
	pointerToolbar->addWidget(analysisOption_);

}

/*! \brief Resets the Designer based on the current DesignRoot.
 *	\details There are a number of actions that result in the element objects contained in the DesignRoot changing.
 *	This function resets the Designer to work with whatever elements are contained in the currently loaded DesignRoot.
 *	\sa loadDesign(), resetExperiment(), performUndoAction(), performRedoAction()
 */
bool Designer::resetEditor()
{
	//Reset the root of the experiment
	editorState_->setTopLevelAsset(designRoot_->getExperiment());

	//Update the analysis list
	analysisOption_->updateLists(true);

	//Reset the window asset
	QSharedPointer<Asset> openAsset = designRoot_->getOpenAsset();
	if(!openAsset)
		openAsset = editorState_->getTopLevelAsset();
	editorState_->setWindowAsset(openAsset,false);
	return true;
}

/*! \brief Updates the Designer actions that are enabled based on the currently Designer context.
 *	\details Which actions are enabled depends on what element is currently selected as well as whether the Designer is
 *	operating in Experiment or Analysis mode.  This updates the available actions accordingly.
 */
void Designer::updateEnabledActions()
{
	QSharedPointer<Asset> asset = editorState_->getSelectedAsset();
	if(editorState_->getCurrentAnalysis())
	{	//An Analysis is active
		analysisExportAction->setVisible(true);
		analysisImportAction->setVisible(true);
		if(!asset)
		{
			deleteAction->setEnabled(false);
			copyAction->setEnabled(false);
			analysisExportAction->setEnabled(false);
			analysisImportAction->setEnabled(false);
			pasteAction->setEnabled(false);
		}
		else if(dynamic_cast<AssociateElement*>(asset.data()))
		{	//The asset is an analysis element
			deleteAction->setEnabled(true);
			copyAction->setEnabled(true);
			analysisExportAction->setEnabled(false);
			analysisImportAction->setEnabled(false);
			pasteAction->setEnabled(false);
		}
		else	//The asset is an experiment element
		{
			deleteAction->setEnabled(false);
			copyAction->setEnabled(false);
			analysisExportAction->setEnabled(asset->inherits("Picto::StateMachineElement") || asset->inherits("Picto::Task"));
			analysisImportAction->setEnabled((asset->inherits("Picto::StateMachineElement") || asset->inherits("Picto::Task")) && (Copier::availablePasteType() == Copier::ANALYSIS_IMPORT));
			pasteAction->setEnabled(false);		
		}
	}
	else	//No Analysis is active
	{
		analysisExportAction->setVisible(false);
		analysisImportAction->setVisible(false);

		if(!asset)
		{
			deleteAction->setEnabled(false);
			copyAction->setEnabled(false);
			analysisExportAction->setEnabled(false);
			analysisImportAction->setEnabled(false);
			pasteAction->setEnabled(false);
		}
		else
		{
			deleteAction->setEnabled(true);
			copyAction->setEnabled(true);
			analysisExportAction->setEnabled(false);
			analysisImportAction->setEnabled(false);
			pasteAction->setEnabled((Copier::availablePasteType() == Copier::EXPERIMENT_PASTE));		
		}
	}
	//If the window is the selected asset, copy should be disabled, 
	//paste depends on whether we're in analysis mode
	if(asset && (asset == editorState_->getWindowAsset()))
	{
		copyAction->setEnabled(false);
		analysisExportAction->setEnabled(false);
		analysisImportAction->setEnabled(false);
		bool pasteEnabled = (editorState_->getCurrentAnalysis().isNull() && (Copier::availablePasteType() == Copier::EXPERIMENT_PASTE))
			|| (!editorState_->getCurrentAnalysis().isNull() && (Copier::availablePasteType() == Copier::ANALYSIS_PASTE));
		pasteAction->setEnabled(pasteEnabled);
	}
	//If the window is the selected asset, and the selected item isn't a transition delete should be disabled
	if	(	asset && (editorState_->getSelectedAsset() == editorState_->getWindowAsset()) 
		&& (dynamic_cast<Arrow*>(editorState_->getSelectedItem()) == NULL)
		)
	{
		deleteAction->setEnabled(false);
	}
}

/*! \brief Called when an undo is first available to enable the Designer's undo action.*/
void  Designer::undoAvailable(bool available)
{
	undoAction->setEnabled(available);
}
/*! \brief Called when a redo is first available to enable the Designer's redo action.*/
void  Designer::redoAvailable(bool available)
{
	redoAction->setEnabled(available);
}

/*! \brief Called when text in the search box changes to initiate a search for the input string.
 *	\details Whenever the text in the searchbox is updated, a search is initiated within the Experimental and Active
 *	Analysis assets.
 */
void Designer::searchTextChanged(const QString& text)
{
	editorState_->requestSearch(SearchRequest(SearchRequest::EXPERIMENT,SearchRequest::STRING,true,text,matchCase->isChecked()));
	editorState_->requestSearch(SearchRequest(SearchRequest::ACTIVE_ANALYSES,SearchRequest::STRING,true,text,matchCase->isChecked()));
}

/*! \brief Called when the matchCase checkbox changes to update the active search using searchTextChanged().
 */
void Designer::matchCaseChanged(int)
{
	if(!searchBox->text().isEmpty())
		searchTextChanged(searchBox->text());
}
