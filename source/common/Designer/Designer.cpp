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
	QToolButton *upButton = new QToolButton;
    upButton->setIcon(QIcon(":/icons/levelup.png"));
	//upButton->setIconSize(QSize(50, 50));
	upButton->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
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
	if(design_)
	{
		disconnect(design_.data(),SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
		disconnect(design_.data(),SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));
	}
	design_ = designRoot->getDesign(identifier,index);
	Q_ASSERT(design_);
	connect(design_.data(),SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
	connect(design_.data(),SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));
	resetRootAndWindowAssets();
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
	resetRootAndWindowAssets();
}

void Designer::insertEditBlock()
{
	design_->setUndoPoint();
}

void Designer::performUndoAction()
{
	design_->undo();
	resetRootAndWindowAssets();
}
void Designer::performRedoAction()
{
	design_->redo();
	resetRootAndWindowAssets();
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
	pointerToolbar->addAction(checkSyntaxAction_);
//! [27]
}

bool Designer::resetRootAndWindowAssets()
{
	//scene->clear();
	//QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(designText_.toPlainText()));

	//QSharedPointer<DataStore> topAsset = createTopLevelAsset();
	////read until we either see an PictoData tag, or the end of the file
	//while(!xmlReader->isStartElement() && (xmlReader->name() != topAsset->assetType())) 
	//	xmlReader->readNext();

	//Q_ASSERT(!xmlReader->atEnd());

	//if(!topAsset->fromXml(xmlReader,false))
	//{
	//	return false;
	//	Q_ASSERT_X(false,"Designer::resetRootAndWindowAssets",Serializable::getErrors().toLatin1());
	//}

	//QSharedPointer<Asset> windowAsset = findAssetWithID(topAsset,openedAssetText_.toPlainText().toInt());

	editorState_->setTopLevelAsset(design_->getRootAsset());
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
