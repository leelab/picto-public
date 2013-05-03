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
#include "PropertyBrowser.h"
#include "AssetInfoBox.h"
#include "ViewerWindow.h"
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
	assetInfoBox_ = new AssetInfoBox(editorState_);
	infoPane_->addTab(assetInfoBox_,"Scripting Info");
	syntaxErrorBox_ = new SyntaxErrorBox(editorState_);
	infoPane_->addTab(syntaxErrorBox_,"Error List");
	aboutElementBox_ = new AboutElementBox(editorState_);
	infoPane_->addTab(aboutElementBox_,"About Element");

	infoPane_->setTabEnabled(SCRIPTINFO,true);
	infoPane_->setTabEnabled(SYNTAXCHECK,false);
	infoPane_->setTabEnabled(ABOUTELEMENT,true);

    centralLayout->addWidget(upButton);
	centralLayout->setStretchFactor(0,0);
	centralLayout->addWidget(view);
	centralLayout->setStretchFactor(1,10);
	centralLayout->addWidget(infoPane_);
	centralLayout->setStretchFactor(2,5);
	//QWidget* centralLayoutWidget = new QWidget();
	//centralLayoutWidget->setLayout(centralLayout);

	toolbox_ = new Toolbox(editorState_);
	splitter->addWidget(toolbox_);
    splitter->addWidget(centralLayout);

	//QSplitter* rightSideWidget = new QSplitter();
	//rightSideWidget->setOrientation(Qt::Vertical);
	propertyEditor_ = new PropertyBrowser(editorState_);
	//rightSideWidget->addWidget(propertyEditor_);
	//rightSideWidget->addWidget(assetInfoBox_);
	//rightSideWidget->setStretchFactor(0,10);
	splitter->addWidget(propertyEditor_);
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
		analysisSelector_->clear();
		analysisSelector_->addItem("None");
		for(int i=0;i<designRoot_->getNumAnalyses();i++)
		{
			analysisSelector_->addItem(designRoot_->getAnalysis(i)->getName());
		}
		analysisSelector_->addItem("New...");
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

    toFrontAction = new QAction(QIcon(":/icons/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));

    sendBackAction = new QAction(QIcon(":/icons/sendtoback.png"),
                                 tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+B"));
    sendBackAction->setStatusTip(tr("Send item to back"));

    deleteAction = new QAction(QIcon(":/icons/delete.png"),
                               tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));

	experimentalCopyAction = new QAction(QIcon(":/icons/copy_experiment.png"),
                               tr("Copy Experiment Elements"), this);
    experimentalCopyAction->setShortcut(tr("Ctrl+C"));
    experimentalCopyAction->setStatusTip(tr("Copy experimental elements from your design"));

	analysisCopyAction = new QAction(QIcon(":/icons/copy_analysis.png"),
                               tr("Copy Analysis Elements"), this);
    analysisCopyAction->setStatusTip(tr("Copy analysis elements from your design"));

	pasteAction = new QAction(QIcon(":/icons/paste.png"),
								tr("Paste"), this);
	pasteAction->setShortcut(tr("Ctrl+V"));
	pasteAction->setStatusTip(tr("Paste copied experimental or analysis elements"));

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

	analysisSelector_ = new QComboBox();
	selectedIndex_ = -1;
	analysisSelector_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	analysisSelector_->setInsertPolicy(QComboBox::NoInsert);
	analysisSelector_->setEditable(false);
	analysisSelector_->addItem("None");
	analysisSelector_->addItem("New...");
	connect(analysisSelector_,SIGNAL(currentIndexChanged(int)),this,SLOT(analysisSelectedChanged(int)));
	connect(analysisSelector_,SIGNAL(editTextChanged(const QString&)),this,SLOT(analysisSelectedTextChanged(const QString&)));
	deleteAnalysisAction_ = new QAction(QIcon(":/icons/delete.png"),
                               tr("De&lete"), this);
    deleteAnalysisAction_->setStatusTip(tr("Delete Analysis"));
	deleteAnalysisAction_->setEnabled(false);
	connect(deleteAnalysisAction_,SIGNAL(triggered()),this,SLOT(deleteCurrentAnalysis()));

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

	connect(toFrontAction, SIGNAL(triggered()),scene, SLOT(bringToFront()));
    connect(sendBackAction, SIGNAL(triggered()),scene, SLOT(sendToBack()));
    connect(deleteAction, SIGNAL(triggered()),scene, SLOT(deleteSelectedItems()));
	connect(experimentalCopyAction, SIGNAL(triggered()),scene, SLOT(experimentalCopySelectedItems()));
	connect(analysisCopyAction, SIGNAL(triggered()),scene, SLOT(analysisCopySelectedItems()));
	connect(pasteAction, SIGNAL(triggered()),scene, SLOT(pasteItems()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(aboutAction, SIGNAL(triggered()),this, SLOT(about()));
	connect(editorState_.data(),SIGNAL(undoableActionPerformed()),this,SLOT(insertEditBlock()));
	connect(editorState_.data(),SIGNAL(windowAssetChanged(QSharedPointer<Asset>)),this,SLOT(setOpenAsset(QSharedPointer<Asset>)));
}

//! [24]
void Designer::createMenus()
{
    //fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(exitAction);

    //itemMenu = menuBar()->addMenu(tr("&Item"));
	itemMenu = new QMenu(tr("&Item"),this);
    itemMenu->addAction(deleteAction);
	itemMenu->addAction(experimentalCopyAction);
	itemMenu->addAction(analysisCopyAction);
	itemMenu->addAction(pasteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

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
	editToolBar->addAction(experimentalCopyAction);
	editToolBar->addAction(analysisCopyAction);
	editToolBar->addAction(pasteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);
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
	pointerToolbar->addWidget(new QLabel(tr("Select Analysis")));
	pointerToolbar->addWidget(analysisSelector_);
	pointerToolbar->addAction(deleteAnalysisAction_);
//! [27]
}

bool Designer::resetEditor()
{
	//Reset the root of the experiment
	editorState_->setTopLevelAsset(designRoot_->getExperiment());

	//Get the selected Analysis and use it in the editor
	int analysisComboboxIndex = analysisSelector_->currentIndex();
	if(analysisComboboxIndex > 0)
	{
		QSharedPointer<Asset> analysis = designRoot_->getAnalysis(analysisComboboxIndex-1);
		Q_ASSERT(analysis);
		editorState_->setCurrentAnalysis(analysis.staticCast<Analysis>());
	}

	//Reset the window asset
	QSharedPointer<Asset> openAsset = designRoot_->getOpenAsset();
	if(!openAsset)
		openAsset = editorState_->getTopLevelAsset();
	editorState_->setWindowAsset(openAsset,false);
	return true;
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
}

void Designer::matchCaseChanged(int)
{
	if(!searchBox->text().isEmpty())
		searchTextChanged(searchBox->text());
}

void Designer::analysisSelectedChanged(int index)
{
	if(index < 0)
		return;
	analysisSelector_->setEditable(false);
	bool newAnalysisCreated = false;
	switch(index)
	{
	case 0:
		if(editorState_)
		{
			editorState_->setCurrentAnalysis(QSharedPointer<Analysis>());
		}
		deleteAnalysisAction_->setEnabled(false);
		break;
	default:
		deleteAnalysisAction_->setEnabled(true);
		//Allow user to edit the current analysis name in the combobox.
		analysisSelector_->setEditable(true);
		if(editorState_)
		{
			//If we need to create a new analysis, do it
			newAnalysisCreated = (index == analysisSelector_->count()-1);
			if(newAnalysisCreated)
			{
				QSharedPointer<Asset> newAnalysis;
				int result = QMessageBox::question(NULL,"Import from file?","Would you like to import the analysis from another design file?",QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
				switch(result)
				{
				case QMessageBox::No:
					{
						//Add a new Analysis Design to the designRoot
						newAnalysis = designRoot_->importAnalysis("<Analysis/>");
						//Create UI Data for the new Analysis and attach it
						AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
						Q_ASSERT(assocRootHost);
						QUuid hostId = assocRootHost->getHostId();
						QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData",QString(),QString());
						QString feedback;
						newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis,feedback);
					}
					break;
				case QMessageBox::Yes:
					//Import an Analysis... not yet implemented
					QMessageBox::information(NULL,"Analysis Import Not Yet Implemented","Importing of an Analysis from a separate file has not yet been implemented");
					analysisSelector_->setCurrentIndex(selectedIndex_);
					return;
					//Load the Analysis and Attached UIData into a new DesignRoot
					//to force version upgrade.

					//Get the Analysis and Attached UIData text from new DesignRoot
					
					//Use copy function to put the analysis and UIData into our design root while keeping them attached

					//Put the new analysis into the newAnalysis variable...
					break;
				default:
					//Canceled, return to previous selection
					analysisSelector_->setCurrentIndex(selectedIndex_);
					return;
				};
				Q_ASSERT(newAnalysis);

				//Attempt to add the new analysis to the EditorState as current
				if(!editorState_->setCurrentAnalysis(newAnalysis.staticCast<Analysis>()))
				{
					//If the analysis couldn't be added as current, remove the Analysis and go back to the last selected
					//analysis.
					designRoot_->removeAnalysis(designRoot_->getNumAnalyses()-1);
					analysisSelector_->setCurrentIndex(selectedIndex_);
					return;
				}

				//Add a new "New Analysis" item to the end of the analysisSelector
				analysisSelector_->addItem("New...");

				//Set the combo boxes value to a default "untitled" string.  This will have the affect
				//of updating the analysis name as well.
				analysisSelector_->setItemText(index, "Untitled");

				//Highlight the "untitled" name
				analysisSelector_->lineEdit()->selectAll();
			}
		}
		break;
	};

	//Attempt to set the selected analysis as the current one, if it doesn't work, go back to the previous analysis.
	if(index > 0)
	{
		QSharedPointer<Asset> analysis = designRoot_->getAnalysis(index-1);
		Q_ASSERT(analysis);
		if(!editorState_->setCurrentAnalysis(analysis.staticCast<Analysis>()))
		{
			analysisSelector_->setCurrentIndex(selectedIndex_);
			return;
		}
	}

	selectedIndex_ = index;
	//Reset the scene so that analysis stuff will come up.
	resetEditor();

	if(newAnalysisCreated)
	{
		//Highlight the "untitled" name
		analysisSelector_->lineEdit()->selectAll();
	}
}

void Designer::analysisSelectedTextChanged(const QString&)
{
	if(analysisSelector_->currentIndex() > 0 && editorState_ && editorState_->getCurrentAnalysis())
	{
		QString newName = analysisSelector_->currentText();
		analysisSelector_->setItemText(analysisSelector_->currentIndex(),newName);
		//This function can get called due to a change in selected index before analysisSelectedChange() is called.
		//This would mean that the editorState's currentAnalysis would not correspond to the analysisSelector_->currentIndex()
		//We avoid this issue by only resetting the analysis's name if it's index corresponds to the current
		//combobox index
		if(selectedIndex_ == analysisSelector_->currentIndex())
			editorState_->getCurrentAnalysis()->setName(newName);
	}
}

void Designer::deleteCurrentAnalysis()
{
	if(!editorState_ || !editorState_->getCurrentAnalysis())
		return;

	int ret = QMessageBox::warning(this, tr("Picto"),
                                QString("Are you sure you want to delete the %1 Analysis?").arg(editorState_->getCurrentAnalysis()->getName()),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel);
	if(ret != QMessageBox::Yes)
		return;
	//Detach analysis from experiment
	QUuid currAnalsisId = editorState_->getCurrentAnalysis()->getAssociateId();
	editorState_->getTopLevelAsset().staticCast<DataStore>()->ClearAssociateDescendants(currAnalsisId);
	
	//Remove analysis from design
	int anaIndexInComboBox = analysisSelector_->currentIndex();
	if(!designRoot_->removeAnalysis(anaIndexInComboBox-1))
		Q_ASSERT(false);

	//Remove analysis name from combobox and set current analysis to none.
	analysisSelector_->setCurrentIndex(0);
	analysisSelector_->removeItem(anaIndexInComboBox);
}