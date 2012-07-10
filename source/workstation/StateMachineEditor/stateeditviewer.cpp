/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QLabel>
#include <QMenuBar>

#include "toolbox.h"
#include "stateeditviewer.h"
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
StateEditViewer::StateEditViewer(QWidget *parent) :
	Viewer(parent),
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

    QHBoxLayout *layout = new QHBoxLayout;
	QVBoxLayout *centralLayout = new QVBoxLayout;
	QToolButton *upButton = new QToolButton;
    upButton->setIcon(QIcon(":/icons/levelup.png"));
	//upButton->setIconSize(QSize(50, 50));
	upButton->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	connect(upButton,SIGNAL(pressed()),editorState_.data(),SLOT(setWindowAssetToParent()));
    centralLayout->addWidget(upButton);
	centralLayout->addWidget(view);

	toolbox_ = new Toolbox(editorState_);
	layout->addWidget(toolbox_);
    layout->addLayout(centralLayout);

	QVBoxLayout * rLayout = new QVBoxLayout;
	propertyEditor_ = new PropertyBrowser(editorState_);
	assetInfoBox_ = new AssetInfoBox(editorState_);
	rLayout->addWidget(propertyEditor_);
	rLayout->addWidget(assetInfoBox_);
	QWidget* rightSideWidget = new QWidget();
	rightSideWidget->setLayout(rLayout);
	layout->addWidget(rightSideWidget);
	layout->setStretch(1,10);

	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(layout);


	setLayout(mainLayout);
}

StateEditViewer::~StateEditViewer()
{
	int i=0;
	i++;
}
//! [0]
void StateEditViewer::init()
{
	if(!pictoData_ || !pictoData_->validateTree())
	{
		QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(pictoDataText_->toPlainText()));

		//read until we either see an experiment tag, or the end of the file
		while(xmlReader->name() != "PictoData" && !xmlReader->atEnd()) 
			xmlReader->readNext();

		if(xmlReader->atEnd())
		{
			QMessageBox msg;
			msg.setText("PictoData XML did not contain <PictoData> tag");
			msg.exec();
			return;
		}

		pictoData_ = QSharedPointer<Picto::PictoData>(Picto::PictoData::Create().staticCast<PictoData>());
		Picto::Asset::clearErrors();
		
		if(!pictoData_->fromXml(xmlReader,false))
		{
			pictoData_ = QSharedPointer<Picto::PictoData>();
			QMessageBox msg;
			msg.setText("Failed to load current definition.  Please attempt to correct PictoData XML in Text Editor");
			msg.exec();
			return;
		}

	}
	editorState_->setPictoDataObject(pictoData_);
}

void StateEditViewer::deinit()
{
	if(pictoData_.isNull())
		return;
	insertEditBlock();
}

void StateEditViewer::aboutToSave()
{
	insertEditBlock();
}

void StateEditViewer::updateEditModeButtons(int id)
{
	pointerTypeGroup->button(id)->setChecked(true);
}

void StateEditViewer::itemInserted(DiagramItem *)
{
    pointerTypeGroup->button(int(DiagramScene::Select))->setChecked(true);
    editorState_->setEditMode(EditorState::EditMode(pointerTypeGroup->checkedId()));
    
}

void StateEditViewer::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
    editorState_->setEditMode(EditorState::EditMode(pointerTypeGroup->checkedId()));
}

void StateEditViewer::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
	editorState_->setZoom(newScale);
}

void StateEditViewer::about()
{
    QMessageBox::about(this, tr("About State Machine Editor"),
                       tr("The <b>State Machine Editor</b> is used"
                          "to design Picto experiments."));
}

void StateEditViewer::loadScene(DiagramScene* newScene)
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

void StateEditViewer::resetExperiment()
{
	insertEditBlock();
	reloadPictoDataFromDoc();
}

void StateEditViewer::insertEditBlock()
{
	Q_ASSERT(pictoDataText_);
	QString newText = pictoData_->toXml();
	if(newText == pictoDataText_->toPlainText())
		return;

	//These operations make the change undoable
	QTextCursor cursor = QTextCursor(pictoDataText_);
	cursor.beginEditBlock();
	cursor.select(QTextCursor::Document);
	cursor.removeSelectedText();
	cursor.insertText(newText);
	cursor.endEditBlock();
}

void StateEditViewer::performUndoAction()
{
	if(!pictoDataText_->isUndoAvailable())
		return;
	pictoDataText_->undo();
	pictoDataText_->redo();
	reloadPictoDataFromDoc();
	QUuid openedID = pictoData_->getStateMachineEditorData()->getOpenedAssetId();
	pictoDataText_->undo();
	reloadPictoDataFromDoc();
	//This puts the window where it should be.  Otherwise we would end up looking at the window
	//that we were at before the undo took place.
	pictoData_->getStateMachineEditorData()->setOpenedAssetId(openedID);
	editorState_->setPictoDataObject(pictoData_);
}
void StateEditViewer::performRedoAction()
{
	if(!pictoDataText_->isRedoAvailable())
		return;
	pictoDataText_->redo();
	reloadPictoDataFromDoc();
}

//! [23]
void StateEditViewer::createActions()
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
}

void StateEditViewer::connectActions()
{
	connect(undoAction, SIGNAL(triggered()),this, SLOT(performUndoAction()));
	connect(redoAction, SIGNAL(triggered()),this, SLOT(performRedoAction()));
	connect(pictoDataText_,SIGNAL(undoAvailable(bool)),undoAction,SLOT(setEnabled(bool)));
	connect(pictoDataText_,SIGNAL(redoAvailable(bool)),redoAction,SLOT(setEnabled(bool)));

	connect(toFrontAction, SIGNAL(triggered()),scene, SLOT(bringToFront()));
    connect(sendBackAction, SIGNAL(triggered()),scene, SLOT(sendToBack()));
    connect(deleteAction, SIGNAL(triggered()),scene, SLOT(deleteSelectedItems()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(boldAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(italicAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(underlineAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(aboutAction, SIGNAL(triggered()),this, SLOT(about()));
	connect(editorState_.data(),SIGNAL(undoableActionPerformed()),this,SLOT(insertEditBlock()));
}

//! [24]
void StateEditViewer::createMenus()
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
void StateEditViewer::createToolbars()
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
//! [27]
}

void StateEditViewer::reloadPictoDataFromDoc()
{

	scene->clear();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(pictoDataText_->toPlainText()));

	//read until we either see an PictoData tag, or the end of the file
	while(xmlReader->name() != "PictoData" && !xmlReader->atEnd()) 
		xmlReader->readNext();

	Q_ASSERT(!xmlReader->atEnd());

	pictoData_ = QSharedPointer<Picto::PictoData>(Picto::PictoData::Create().staticCast<PictoData>());

	if(!pictoData_->fromXml(xmlReader,false))
	{
		Q_ASSERT_X(false,"StateEditViewer::reloadPictoDataFromDoc",Serializable::getErrors().toAscii());
	}
	
	editorState_->setPictoDataObject(pictoData_);
	return;
}
