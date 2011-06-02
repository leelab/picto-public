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

const int InsertTextButton = 10;

//! [0]
StateEditViewer::StateEditViewer(QWidget *parent) :
	Viewer(parent)
{
	scene = NULL;
	view = NULL;
	createActions();
	createMenus();
    createToolbars();
	topmostScene = new DiagramScene(itemMenu);
	loadScene(topmostScene);
	connectActions();
	
	QVBoxLayout *mainLayout = new QVBoxLayout;

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(editToolBar);
	toolbarLayout->addWidget(textToolBar);
    toolbarLayout->addWidget(colorToolBar);
    toolbarLayout->addWidget(pointerToolbar);

    QHBoxLayout *layout = new QHBoxLayout;

	toolbox_ = new Toolbox;
	connect(toolbox_, SIGNAL(insertionItemSelected(QString)),scene, SLOT(setInsertionItem(QString)));
	layout->addWidget(toolbox_);
    layout->addWidget(view);

	variantFactory_ = QSharedPointer<QtVariantEditorFactory>(new QtVariantEditorFactory());
	variantEditor_ = new QtGroupBoxPropertyBrowser();
	layout->addWidget(variantEditor_);
	layout->setStretch(1,10);

	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(layout);


	setLayout(mainLayout);
    //QWidget *widget = new QWidget;
    //widget->setLayout(layout);

    //setCentralWidget(widget);
    //setWindowTitle(tr("Diagramscene"));
}
//! [0]
void StateEditViewer::init()
{
	if(!experiment_)
	{
		QMessageBox msg;
		msg.setText("Failed to load current experiment.");
		msg.exec();
	}
	topmostScene->setSceneAsset(experiment_.staticCast<Asset>());
}
//! [1]
//void StateEditViewer::backgroundButtonGroupClicked(QAbstractButton *button)
//{
//    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
//    foreach (QAbstractButton *myButton, buttons) {
//    if (myButton != button)
//        button->setChecked(false);
//    }
//    QString text = button->text();
//    if (text == tr("Blue Grid"))
//        scene->setBackgroundBrush(QPixmap(":/icons/background1.png"));
//    else if (text == tr("White Grid"))
//        scene->setBackgroundBrush(QPixmap(":/icons/background2.png"));
//    else if (text == tr("Gray Grid"))
//        scene->setBackgroundBrush(QPixmap(":/icons/background3.png"));
//    else
//        scene->setBackgroundBrush(QPixmap(":/icons/background4.png"));
//
//    scene->update();
//    view->update();
//}

//! [4]
void StateEditViewer::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

//! [6]

//! [7]
void StateEditViewer::itemInserted(DiagramItem *)
{
    pointerTypeGroup->button(int(DiagramScene::Select))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    
}
//! [7]

//! [8]
void StateEditViewer::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}
//! [8]

//! [9]
void StateEditViewer::currentFontChanged(const QFont &)
{
    handleFontChange();
}
//! [9]

//! [10]
void StateEditViewer::fontSizeChanged(const QString &)
{
    handleFontChange();
}
//! [10]

//! [11]
void StateEditViewer::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}
//! [11]

//! [12]
void StateEditViewer::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                ":/icons/textpointer.png",
                qVariantValue<QColor>(textAction->data())));
    textButtonTriggered();
}
//! [12]

//! [13]
void StateEditViewer::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                 ":/icons/floodfill.png",
                 qVariantValue<QColor>(fillAction->data())));
    fillButtonTriggered();
}
//! [13]

//! [14]
void StateEditViewer::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                 ":/icons/linecolor.png",
                 qVariantValue<QColor>(lineAction->data())));
    lineButtonTriggered();
}
//! [14]

//! [15]
void StateEditViewer::textButtonTriggered()
{
    scene->setTextColor(qVariantValue<QColor>(textAction->data()));
}
//! [15]

//! [16]
void StateEditViewer::fillButtonTriggered()
{
    scene->setItemColor(qVariantValue<QColor>(fillAction->data()));
}
//! [16]

//! [17]
void StateEditViewer::lineButtonTriggered()
{
    scene->setLineColor(qVariantValue<QColor>(lineAction->data()));
}
//! [17]

//! [18]
void StateEditViewer::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}
//! [18]
void StateEditViewer::assetSelected(QSharedPointer<Asset> asset)
{
	loadAssetProperties(asset);
}

//! [19]
void StateEditViewer::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

	if(textItem)
	{
		QFont font = textItem->font();
		QColor color = textItem->defaultTextColor();
		fontCombo->setCurrentFont(font);
		fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
		boldAction->setChecked(font.weight() == QFont::Bold);
		italicAction->setChecked(font.italic());
		underlineAction->setChecked(font.underline());
		return;
	}
	AssetItem *assetItem = qgraphicsitem_cast<AssetItem *>(item);
	if(assetItem)
	{
		loadAssetProperties(assetItem->getAsset());
	}
}
//! [19]

//! [20]
void StateEditViewer::about()
{
    QMessageBox::about(this, tr("About State Machine Editor"),
                       tr("The <b>State Machine Editor</b> is used"
                          "to design Picto experiments."));
}
//! [20]

void StateEditViewer::loadScene(DiagramScene* newScene)
{
	if(scene == newScene)
		return;

	//Unload old scene
	//if(scene)
	//{
	//	disconnect(scene, SIGNAL(itemInserted(DiagramItem *)),
	//		this, SLOT(itemInserted(DiagramItem *)));
	//	disconnect(scene, SIGNAL(textInserted(QGraphicsTextItem *)),
	//		this, SLOT(textInserted(QGraphicsTextItem *)));
	//	disconnect(scene, SIGNAL(itemSelected(QGraphicsItem *)),
	//		this, SLOT(itemSelected(QGraphicsItem *)));
	//	disconnect(scene, SIGNAL(sceneAssetChanged(DiagramScene *)),
	//		this, SLOT(loadScene(DiagramScene *)));
	//}

	//Switch Scene
	scene = newScene;

	//Load new Scene
	newScene->setSceneRect(QRectF(0, 0, 4000, 4000));
	if(view == NULL)
		view = new QGraphicsView(newScene);
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
    connect(newScene, SIGNAL(itemSelected(QGraphicsItem *)),
        this, SLOT(itemSelected(QGraphicsItem *)));
	connect(newScene, SIGNAL(assetSelected(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	connect(newScene, SIGNAL(sceneAssetChanged(QSharedPointer<Asset>)),
		this, SLOT(loadAsset(QSharedPointer<Asset>)));
}

void StateEditViewer::loadAsset(QSharedPointer<Asset> asset)
{
	toolbox_->setAsset(asset);
	loadAssetProperties(asset);
}

void StateEditViewer::loadAssetProperties(QSharedPointer<Asset> asset)
{
	variantEditor_->clear();
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	variantEditor_->setFactoryForManager(dataStore->getPropertyContainer()->getPropertyManager().data(), variantFactory_.data());
	QMap<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	for(QMap<QString, QVector<QSharedPointer<Property>>>::iterator typeIter = properties.begin();typeIter !=properties.end();typeIter ++)
	{
		for(QVector<QSharedPointer<Property>>::iterator propIter = typeIter.value().begin(); propIter != typeIter.value().end(); propIter++)
		{
			variantEditor_->addProperty((*propIter)->getVariantProperty().data());
		}
	}
	variantEditor_->setMinimumWidth(variantEditor_->childrenRect().width());
}

//! [23]
void StateEditViewer::createActions()
{
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
	connect(toFrontAction, SIGNAL(triggered()),scene, SLOT(bringToFront()));
    connect(sendBackAction, SIGNAL(triggered()),scene, SLOT(sendToBack()));
    connect(deleteAction, SIGNAL(triggered()),scene, SLOT(deleteSelectedItems()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(boldAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(italicAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(underlineAction, SIGNAL(triggered()),this, SLOT(handleFontChange()));
    connect(aboutAction, SIGNAL(triggered()),this, SLOT(about()));
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

    fontCombo = new QFontComboBox();
    fontSizeCombo = new QComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(currentFontChanged(const QFont &)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(fontSizeChanged(const QString &)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()),
                                                 Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(
    ":/icons/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

//! [26]
    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()),
                         Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
    ":/icons/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));
//! [26]

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()),
                                 Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
        ":/icons/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

    textToolBar = new QToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = new QToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/icons/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/icons/linepointer.png"));

    pointerTypeGroup = new QButtonGroup;
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::Select));
    pointerTypeGroup->addButton(linePointerButton,
                                int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    pointerToolbar = new QToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
//! [27]
}
//! [27]

//! [28]
QWidget *StateEditViewer::createBackgroundCellWidget(const QString &text,
                        const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
//! [28]

//! [29]
QWidget *StateEditViewer::createCellWidget(const QString &text,
                      DiagramItem::DiagramType type)
{
	// JOEY TOOK THE TYPE OUT BELOW... We will use the factory to replace this functionality
    DiagramItem item(/*type, */itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
//! [29]

//! [30]
QMenu *StateEditViewer::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu;
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()),
                this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor) {
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}
//! [30]

//! [31]
QIcon StateEditViewer::createColorToolButtonIcon(const QString &imageFile,
                        QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    QRect target(0, 0, 50, 60);
    QRect source(0, 0, 42, 42);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}
//! [31]

//! [32]
QIcon StateEditViewer::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
//! [32]