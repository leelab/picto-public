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

#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "DiagramItemFactory.h"
#include "EditorState.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
#include <QPointer>
using namespace Picto;

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

//! [0]
class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { Select, Navigate, InsertLine};

    DiagramScene(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QObject *parent = 0);
	virtual ~DiagramScene(){};

	QGraphicsLineItem* insertTransition(DiagramItem* source, DiagramItem* dest, QSharedPointer<Asset> transition = QSharedPointer<Asset>());
	DiagramItem* insertDiagramItem(QSharedPointer<Asset> asset,QPointF pos);
	QRectF getDefaultZoomRect();


public slots:
	void setSceneAsset(QSharedPointer<Asset> asset);
	void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);
	void setBackgroundPattern(QPixmap pattern);
    void editorLostFocus(DiagramTextItem *item);
	void deleteSelectedItems();
	void experimentalCopySelectedItems();
	void analysisCopySelectedItems();
	void bringToFront();
	void sendToBack();

signals:
    void itemInserted(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);
	void assetSelected(QSharedPointer<Asset> asset);
	void sceneAssetChanged(QSharedPointer<Asset> asset);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);

private:
	QSharedPointer<Asset> createNewAsset();
	void insertTextItem(QString text,QPointF pos);
    bool isItemChange(int type);

	QSharedPointer<EditorState> editorState_;
	QSharedPointer<DiagramItemFactory> diagItemFactory_;
    DiagramItem::DiagramType myItemType;
	DiagramItem* startBar_;
	QString insertionItem_;
	int newItemIndex_;
    QMenu *myItemMenu;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    DiagramTextItem *textItem;
};
//! [0]

#endif
