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
	topmostScene = new DiagramScene(editorState_,itemMenu);
	loadScene(topmostScene);
	connectActions();
	
	QVBoxLayout *mainLayout = new QVBoxLayout;

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(editToolBar);
    toolbarLayout->addWidget(pointerToolbar);
	toolbarLayout->addStretch();

    QSplitter *splitter = new QSplitter;
	QVBoxLayout *centralLayout = new QVBoxLayout;
	upButton = new LevelUpButton(editorState_);
	connect(upButton,SIGNAL(pressed()),editorState_.data(),SLOT(setWindowAssetToParent()));
    centralLayout->addWidget(upButton);
	centralLayout->addWidget(view);
	QWidget* centralLayoutWidget = new QWidget();
	centralLayoutWidget->setLayout(centralLayout);

	toolbox_ = new Toolbox(editorState_);
	splitter->addWidget(toolbox_);
    splitter->addWidget(centralLayoutWidget);

	QSplitter* rightSideWidget = new QSplitter();
	rightSideWidget->setOrientation(Qt::Vertical);
	propertyEditor_ = new PropertyBrowser(editorState_);
	assetInfoBox_ = new AssetInfoBox(editorState_);
	rightSideWidget->addWidget(propertyEditor_);
	rightSideWidget->addWidget(assetInfoBox_);
	rightSideWidget->setStretchFactor(0,10);
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
void Designer::loadDesign(QString identifier, int index, QSharedPointer<DesignRoot> designRoot)
{
	Q_ASSERT(designRoot);
	designRoot_ = designRoot;
	if(design_)
	{
		disconnect(design_.data(),SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
		disconnect(design_.data(),SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));
	}
	design_ = designRoot->getDesign(identifier,index);
	Q_ASSERT(design_);
	connect(design_.data(),SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
	connect(design_.data(),SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));

	//Populate the AnalysisSelector
	editorState_->setCurrentAnalysis(QSharedPointer<Analysis>());
	analysisSelector_->clear();
	analysisSelector_->addItem("None");
	for(int i=0;i<designRoot_->getDesignCount("Analysis");i++)
	{
		analysisSelector_->addItem(designRoot_->getDesign("Analysis",i)->getRootAsset()->getName());
	}
	analysisSelector_->addItem("New...");
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

void Designer::updateEditModeButtons(int id)
{
	pointerTypeGroup->button(id)->setChecked(true);
}

void Designer::itemInserted(DiagramItem *)
{
    pointerTypeGroup->button(int(DiagramScene::Select))->setChecked(true);
    editorState_->setEditMode(EditorState::EditMode(pointerTypeGroup->checkedId()));
    
}

void Designer::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
    editorState_->setEditMode(EditorState::EditMode(pointerTypeGroup->checkedId()));
}

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
	connect(newScene, SIGNAL(itemInserted(DiagramItem *)),
            this, SLOT(itemInserted(DiagramItem *)));
    connect(newScene, SIGNAL(textInserted(QGraphicsTextItem *)),
        this, SLOT(textInserted(QGraphicsTextItem *)));
	connect(editorState_.data(),SIGNAL(resetExperiment()),
		this,SLOT(resetExperiment()));
}

void Designer::resetExperiment()
{
	insertEditBlock();
	design_->refreshFromXml();
	resetEditor();
}

void Designer::insertEditBlock()
{
	design_->setUndoPoint();
}

void Designer::performUndoAction()
{
	design_->undo();
	resetEditor();
}
void Designer::performRedoAction()
{
	design_->redo();
	resetEditor();
}

void Designer::setOpenAsset(QSharedPointer<Asset> asset)
{
	Q_ASSERT(asset);
	design_->setOpenAsset(asset);
}

//! Checks the syntax of the current XML to see if it is a legal experiment
void Designer::checkSyntax()
{
	QString errors;
	if(design_->compiles(&errors))
	{
		QMessageBox box;
		box.setText("Syntax check passed");
		box.setIconPixmap(QPixmap(":/icons/check.png"));
		box.exec();
	}
	else
	{
		QMessageBox box;
		box.setText("Syntax check failed                                              "
			"                                                                         "
			"                                                                         "
			"                                                                         ");
		box.setDetailedText(errors);
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
	}
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

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    exitAction->setStatusTip(tr("Quit Scenediagram example"));

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/icons/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));

    italicAction = new QAction(QIcon(":/icons/italic.png"),
                               tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));

    underlineAction = new QAction(QIcon(":/icons/underline.png"),
                                  tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));

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

	//Turn on highlighting for elements with children that have scripts and analysis scripts.  To do this, just search 
	//for any non-empty string.  Searching for an empty string turns it off.
	editorState_->requestSearch(SearchRequest(SearchRequest::EXPERIMENT,SearchRequest::SCRIPT));
	editorState_->requestSearch(SearchRequest(SearchRequest::ANALYSIS,SearchRequest::SCRIPT));

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

	connect(toFrontAction, SIGNAL(triggered()),scene, SLOT(bringToFront()));
    connect(sendBackAction, SIGNAL(triggered()),scene, SLOT(sendToBack()));
    connect(deleteAction, SIGNAL(triggered()),scene, SLOT(deleteSelectedItems()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(boldAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(italicAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(underlineAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
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
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

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
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);
	editToolBar->addAction(undoAction);
	editToolBar->addAction(redoAction);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/icons/pointer.png"));
	QToolButton *navigateButton = new QToolButton;
    navigateButton->setCheckable(true);
    navigateButton->setIcon(QIcon(":/icons/cursor-openhand.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/icons/linepointer.png"));

    pointerTypeGroup = QSharedPointer<QButtonGroup>(new QButtonGroup);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::Select));
	pointerTypeGroup->addButton(navigateButton,
                                int(DiagramScene::Navigate));
    pointerTypeGroup->addButton(linePointerButton,
                                int(DiagramScene::InsertLine));
	pointerTypeGroup->setExclusive(true);
    connect(pointerTypeGroup.data(), SIGNAL(buttonClicked(int)),
            editorState_.data(), SLOT(setEditMode(int)));
	connect(editorState_.data(), SIGNAL(editModeChanged(int)),
            this, SLOT(updateEditModeButtons(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    pointerToolbar = new QToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
	pointerToolbar->addWidget(navigateButton);
    pointerToolbar->addWidget(linePointerButton);
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
	editorState_->setTopLevelAsset(design_->getRootAsset());

	//Get the selected Analysis and use it in the editor
	int analysisComboboxIndex = analysisSelector_->currentIndex();
	if(analysisComboboxIndex > 0)
	{
		QSharedPointer<Design> anaDesign = designRoot_->getDesign("Analysis",analysisComboboxIndex-1);
		Q_ASSERT(anaDesign);
		QSharedPointer<Asset> analysis = anaDesign->getRootAsset();
		editorState_->setCurrentAnalysis(analysis.staticCast<Analysis>());
	}

	//Reset the window asset
	editorState_->setWindowAsset(design_->getOpenAsset());
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
				//Add a new Analysis Design to the designRoot
				QSharedPointer<Design> newDesign = designRoot_->importDesign("Analysis","<Analysis/>");
				Q_ASSERT(newDesign);
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
	selectedIndex_ = index;
	//Reset the scene so that analysis stuff will come up.
	resetEditor();

	if(newAnalysisCreated)
	{
		//Highlight the "untitled" name
		analysisSelector_->lineEdit()->selectAll();
	}
}

void Designer::analysisSelectedTextChanged(const QString& text)
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
	QUuid currAnalsisId = editorState_->getCurrentAnalysis()->getAnalysisId();
	editorState_->getTopLevelAsset().staticCast<DataStore>()->ClearAnalysisDescendants(currAnalsisId);
	
	//Remove analysis from design
	int anaIndexInComboBox = analysisSelector_->currentIndex();
	if(!designRoot_->removeDesign("Analysis",anaIndexInComboBox-1))
		Q_ASSERT(false);

	//Remove analysis name from combobox and set current analysis to none.
	analysisSelector_->setCurrentIndex(0);
	analysisSelector_->removeItem(anaIndexInComboBox);
}