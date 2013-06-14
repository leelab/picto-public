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
#include "../../common/memleakdetect.h"

const int InsertTextButton = 10;

//! [0]
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

    QSplitter *splitter = new QSplitter;
	QSplitter *centralLayout = new QSplitter;
	centralLayout->setOrientation(Qt::Vertical);
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
	//QWidget* centralLayoutWidget = new QWidget();
	//centralLayoutWidget->setLayout(centralLayout);

	toolbox_ = new Toolbox(editorState_);
	splitter->addWidget(toolbox_);
    splitter->addWidget(centralLayout);

	QSplitter* rightSideWidget = new QSplitter();
	rightSideWidget->setOrientation(Qt::Vertical);
	propertyEditor_ = new PropertyBrowser(editorState_);
	notesWidget_ = new ElementNotesWidget(editorState_);
	rightSideWidget->addWidget(propertyEditor_);
	rightSideWidget->addWidget(notesWidget_);
	rightSideWidget->setStretchFactor(0,7);
	rightSideWidget->setStretchFactor(1,3);
	splitter->addWidget(rightSideWidget);
	splitter->setStretchFactor(1,10);

	mainLayout->addLayout(toolbarLayout);
	mainLayout->addWidget(splitter);


	setLayout(mainLayout);
}

Designer::~Designer()
{
}
//! [0]
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

//void Designer::deinit()
//{
//	if(pictoData_.isNull())
//		return;
//	insertEditBlock();
//}

//void Designer::aboutToSave()
//{
//	insertEditBlock();
//}

//void Designer::updateEditModeButtons(int id)
//{
//	pointerTypeGroup->button(id)->setChecked(true);
//}

//void Designer::itemInserted(DiagramItem *)
//{
//    //pointerTypeGroup->button(int(DiagramScene::Select))->setChecked(true);
//    //editorState_->setEditMode(EditorState::EditMode(pointerTypeGroup->checkedId()));
//    
//}
//
//void Designer::textInserted(QGraphicsTextItem *)
//{
//    //buttonGroup->button(InsertTextButton)->setChecked(false);
//    //editorState_->setEditMode(EditorState::EditMode(pointerTypeGroup->checkedId()));
//}

void Designer::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
	editorState_->setZoom(newScale);
}

void Designer::about()
{
    QMessageBox::about(this, tr("About State Machine Editor"),
                       tr("The <b>State Machine Editor</b> is used"
                          "to design Picto experiments."));
}

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
	//In this version of Qt it looks like connecting signals to slots more than
	//once does not have the effect of multiplying the number of times that the
	//slots are called.  In future versions this may occur though because future 
	//versions support a Qt::UniqueConnection parameter.  We would just disconnect
	//old connections before connecting the new ones but this isn't working for 
	//some reason.
	//connect(newScene, SIGNAL(itemInserted(DiagramItem *)),
 //           this, SLOT(itemInserted(DiagramItem *)));
    //connect(newScene, SIGNAL(textInserted(QGraphicsTextItem *)),
    //    this, SLOT(textInserted(QGraphicsTextItem *)));
	connect(editorState_.data(),SIGNAL(resetExperiment()),
		this,SLOT(resetExperiment()));
}

void Designer::resetExperiment()
{
	insertEditBlock();
	designRoot_->refreshFromXml();
	resetEditor();
}

void Designer::insertEditBlock()
{
	designRoot_->setUndoPoint();
}

void Designer::performUndoAction()
{
	if(!designRoot_->hasUndo())
		return;
	designRoot_->undo();
	resetEditor();
}
void Designer::performRedoAction()
{
	if(!designRoot_->hasRedo())
		return;
	designRoot_->redo();
	resetEditor();
}

void Designer::setOpenAsset(QSharedPointer<Asset> asset)
{
	Q_ASSERT(asset);
	designRoot_->setOpenAsset(asset);
}

void Designer::selectedAssetChanged(QSharedPointer<Asset>)
{
	updateEnabledActions();
}

void Designer::selectedItemChanged(QGraphicsItem*)
{
	updateEnabledActions();
}

void Designer::currentAnalysisChanged(QSharedPointer<Analysis>)
{
	//When current analysis changes, reload the window
	QSharedPointer<Asset> openAsset = designRoot_->getOpenAsset();
	if(!openAsset)
		openAsset = editorState_->getTopLevelAsset();
	editorState_->setWindowAsset(openAsset,false);
}

//! Checks the syntax of the current XML to see if it is a legal experiment
void Designer::checkSyntax()
{
	QString errors;
	infoPane_->setTabEnabled(SYNTAXCHECK,true);
	infoPane_->setCurrentIndex(SYNTAXCHECK);
	syntaxErrorBox_->checkSyntax();
}

//! [23]
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

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));

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
    connect(aboutAction, SIGNAL(triggered()),this, SLOT(about()));
	connect(editorState_.data(),SIGNAL(undoableActionPerformed()),this,SLOT(insertEditBlock()));
	connect(editorState_.data(),SIGNAL(windowAssetChanged(QSharedPointer<Asset>)),this,SLOT(setOpenAsset(QSharedPointer<Asset>)));
	connect(editorState_.data(),SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),this,SLOT(selectedAssetChanged(QSharedPointer<Asset>)));
	connect(editorState_.data(),SIGNAL(itemSelected(QGraphicsItem*)),this,SLOT(selectedItemChanged(QGraphicsItem*)));
	connect(editorState_.data(),SIGNAL(currentAnalysisChanged(QSharedPointer<Analysis>)),this,SLOT(currentAnalysisChanged(QSharedPointer<Analysis>)));
}

//! [24]
void Designer::createMenus()
{
    //fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(exitAction);

    //itemMenu = menuBar()->addMenu(tr("&Item"));
	itemMenu = new QMenu(tr("&Item"),this);
    itemMenu->addAction(deleteAction);
	itemMenu->addAction(copyAction);
	itemMenu->addAction(pasteAction);
	itemMenu->addAction(analysisExportAction);
	itemMenu->addAction(analysisImportAction);

	sceneMenu = new QMenu(tr("Scene"),this);
	sceneMenu->addAction(pasteAction);

    //aboutMenu = menuBar()->addMenu(tr("&Help"));
    //aboutMenu->addAction(aboutAction);
}
//! [24]

//! [25]
void Designer::createToolbars()
{
//! [25]
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
//! [27]
}

bool Designer::resetEditor()
{
	//Reset the root of the experiment
	editorState_->setTopLevelAsset(designRoot_->getExperiment());

	//Update the analysis list
	analysisOption_->updateAnalysisList();

	//Reset the window asset
	QSharedPointer<Asset> openAsset = designRoot_->getOpenAsset();
	if(!openAsset)
		openAsset = editorState_->getTopLevelAsset();
	editorState_->setWindowAsset(openAsset,false);
	return true;
}

void Designer::updateEnabledActions()
{
	QSharedPointer<Asset> asset = editorState_->getSelectedAsset();
	if(editorState_->getCurrentAnalysis())
	{	//An Analysis is active
		analysisExportAction->setVisible(true);
		analysisImportAction->setVisible(true);
		if(dynamic_cast<AssociateElement*>(asset.data()))
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

		deleteAction->setEnabled(true);
		copyAction->setEnabled(true);
		analysisExportAction->setEnabled(false);
		analysisImportAction->setEnabled(false);
		pasteAction->setEnabled((Copier::availablePasteType() == Copier::EXPERIMENT_PASTE));		
	}
	//If the window is the selected asset, copy should be disabled, 
	//paste depends on whether we're in analysis mode
	if(editorState_->getSelectedAsset() == editorState_->getWindowAsset())
	{
		copyAction->setEnabled(false);
		analysisExportAction->setEnabled(false);
		analysisImportAction->setEnabled(false);
		bool pasteEnabled = (editorState_->getCurrentAnalysis().isNull() && (Copier::availablePasteType() == Copier::EXPERIMENT_PASTE))
			|| (!editorState_->getCurrentAnalysis().isNull() && (Copier::availablePasteType() == Copier::ANALYSIS_PASTE));
		pasteAction->setEnabled(pasteEnabled);
	}
	//If the window is the selected asset, and the selected item isn't a transition delete should be disabled
	if	(	editorState_->getSelectedAsset() == editorState_->getWindowAsset() 
		&& (dynamic_cast<Arrow*>(editorState_->getSelectedItem()) == NULL)
		)
	{
		deleteAction->setEnabled(false);
	}
}

void  Designer::undoAvailable(bool available)
{
	undoAction->setEnabled(available);
}
void  Designer::redoAvailable(bool available)
{
	redoAction->setEnabled(available);
}

void Designer::searchTextChanged(const QString& text)
{
	editorState_->requestSearch(SearchRequest(SearchRequest::EXPERIMENT,SearchRequest::STRING,true,text,matchCase->isChecked()));
	editorState_->requestSearch(SearchRequest(SearchRequest::ACTIVE_ANALYSES,SearchRequest::STRING,true,text,matchCase->isChecked()));
}

void Designer::matchCaseChanged(int)
{
	if(!searchBox->text().isEmpty())
		searchTextChanged(searchBox->text());
}
//
//void Designer::analysisSelectedChanged(int index)
//{
//	if(index < 0)
//		return;
//	analysisSelector_->setEditable(false);
//	bool newAnalysisCreated = false;
//	switch(index)
//	{
//	case 0:
//		if(editorState_)
//		{
//			editorState_->setCurrentAnalysis(QSharedPointer<Analysis>());
//		}
//		deleteAnalysisAction_->setEnabled(false);
//		break;
//	default:
//		deleteAnalysisAction_->setEnabled(true);
//		//Allow user to edit the current analysis name in the combobox.
//		analysisSelector_->setEditable(true);
//		if(editorState_)
//		{
//			//If we need to create a new analysis, do it
//			newAnalysisCreated = (index == analysisSelector_->count()-1);
//			if(newAnalysisCreated)
//			{
//				QSharedPointer<Asset> newAnalysis;
//
//				//Add a new Analysis Design to the designRoot
//				newAnalysis = designRoot_->importAnalysis("<Analysis/>");
//				//Create UI Data for the new Analysis and attach it
//				AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
//				Q_ASSERT(assocRootHost);
//				QUuid hostId = assocRootHost->getHostId();
//				QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData",QString(),QString());
//				QString feedback;
//				newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis,feedback);
//				Q_ASSERT(newAnalysis);
//
//				//Attempt to add the new analysis to the EditorState as current
//				if(!editorState_->setCurrentAnalysis(newAnalysis.staticCast<Analysis>()))
//				{
//					//If the analysis couldn't be added as current, remove the Analysis and go back to the last selected
//					//analysis.
//					designRoot_->removeAnalysis(designRoot_->getNumAnalyses()-1);
//					analysisSelector_->setCurrentIndex(selectedIndex_);
//					return;
//				}
//
//				//Add a new "New Analysis" item to the end of the analysisSelector
//				analysisSelector_->addItem("New...");
//
//				//Set the combo boxes value to a default "untitled" string.  This will have the affect
//				//of updating the analysis name as well.
//				analysisSelector_->setItemText(index, "Untitled");
//
//				//Highlight the "untitled" name
//				analysisSelector_->lineEdit()->selectAll();
//			}
//		}
//		break;
//	};
//
//	//Attempt to set the selected analysis as the current one, if it doesn't work, go back to the previous analysis.
//	if(index > 0)
//	{
//		QSharedPointer<Asset> analysis = designRoot_->getAnalysis(index-1);
//		Q_ASSERT(analysis);
//		if(!editorState_->setCurrentAnalysis(analysis.staticCast<Analysis>()))
//		{
//			analysisSelector_->setCurrentIndex(selectedIndex_);
//			return;
//		}
//	}
//
//	selectedIndex_ = index;
//	//Reset the scene so that analysis stuff will come up.
//	resetEditor();
//
//	if(newAnalysisCreated)
//	{
//		//Highlight the "untitled" name
//		analysisSelector_->lineEdit()->selectAll();
//	}
//}
//
//void Designer::analysisSelectedTextChanged(const QString&)
//{
//	if(analysisSelector_->currentIndex() > 0 && editorState_ && editorState_->getCurrentAnalysis())
//	{
//		QString newName = analysisSelector_->currentText();
//		analysisSelector_->setItemText(analysisSelector_->currentIndex(),newName);
//		//This function can get called due to a change in selected index before analysisSelectedChange() is called.
//		//This would mean that the editorState's currentAnalysis would not correspond to the analysisSelector_->currentIndex()
//		//We avoid this issue by only resetting the analysis's name if it's index corresponds to the current
//		//combobox index
//		if(selectedIndex_ == analysisSelector_->currentIndex())
//			editorState_->getCurrentAnalysis()->setName(newName);
//	}
//}
//
//void Designer::deleteCurrentAnalysis()
//{
//	if(!editorState_ || !editorState_->getCurrentAnalysis())
//		return;
//
//	int ret = QMessageBox::warning(this, tr("Picto"),
//                                QString("Are you sure you want to delete the %1 Analysis?").arg(editorState_->getCurrentAnalysis()->getName()),
//                                QMessageBox::Yes | QMessageBox::Cancel,
//                                QMessageBox::Cancel);
//	if(ret != QMessageBox::Yes)
//		return;
//	//Detach analysis from experiment
//	QUuid currAnalsisId = editorState_->getCurrentAnalysis()->getAssociateId();
//	editorState_->getTopLevelAsset().staticCast<DataStore>()->ClearAssociateDescendants(currAnalsisId);
//	
//	//Remove analysis from design
//	int anaIndexInComboBox = analysisSelector_->currentIndex();
//	if(!designRoot_->removeAnalysis(anaIndexInComboBox-1))
//		Q_ASSERT(false);
//
//	//Remove analysis name from combobox and set current analysis to none.
//	analysisSelector_->setCurrentIndex(0);
//	analysisSelector_->removeItem(anaIndexInComboBox);
//}