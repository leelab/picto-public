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

#ifndef DESIGNER_H
#define DESIGNER_H

#include <QMainWindow>
#include <QPointer>
#include <QButtonGroup>



#include "qtpropertymanager.h"
#include "qtvariantproperty.h"



#include "../common/storage/asset.h"
using namespace Picto;

#include "diagramitem.h"
#include "../viewer.h"
#include "EditorState.h"
#include "Design.h"


class DiagramScene;
class Toolbox;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

//! [0]
class Designer : public QWidget
{
    Q_OBJECT

public:
	Designer(QWidget *parent=0);
	virtual ~Designer();

	void loadDesign(QString identifier, int index, QSharedPointer<DesignRoot> designRoot);  //Called just before displaying the viewer
	//virtual void deinit();	//Called just after the user switches out of the viewer
	//virtual void aboutToSave();  //Called just before the pictoDataText_ is saved to file.

private slots:
    void updateEditModeButtons(int id);
    void itemInserted(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void sceneScaleChanged(const QString &scale);
	//void assetSelected(QSharedPointer<Asset> asset);
    //void itemSelected(QGraphicsItem *item);
    void about();
	void loadScene(DiagramScene* newScene);
	void resetExperiment();
	void insertEditBlock();
	void performUndoAction();
	void performRedoAction();
	void setOpenAsset(QSharedPointer<Asset> asset);
	void checkSyntax();

private:
    void createActions();
	void connectActions();
    void createMenus();
    void createToolbars();
	bool resetRootAndWindowAssets();

	QSharedPointer<Design> design_;
	QSharedPointer<EditorState> editorState_;
	//QTextDocument designText_;
	//QTextDocument openedAssetText_;
    DiagramScene *scene;
	DiagramScene *topmostScene;
	Toolbox *toolbox_;
    QGraphicsView *view;

    QAction *exitAction;
    QAction *addAction;
    QAction *deleteAction;

    QAction *toFrontAction;
    QAction *sendBackAction;
    QAction *aboutAction;
	QAction *checkSyntaxAction_;

	QAction *undoAction;
	QAction *redoAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *editToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;

    QSharedPointer<QButtonGroup> buttonGroup;
    QSharedPointer<QButtonGroup> pointerTypeGroup;
    QSharedPointer<QButtonGroup> backgroundButtonGroup;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QAction *boldAction;
    QAction *underlineAction;
    QAction *italicAction;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;

	QWidget *propertyEditor_;
	QWidget *assetInfoBox_;

private slots:
	void  undoAvailable(bool available);
	void  redoAvailable(bool available);
};
//! [0]

#endif
