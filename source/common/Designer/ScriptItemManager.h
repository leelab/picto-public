#ifndef SCRIPTITEMMANAGER_H
#define SCRIPTITEMMANAGER_H

#include <QMap>
#include <QVector>
#include <QColor>
#include "ScriptItem.h"

//! [0]
class ScriptItemManager : public QObject
{
	Q_OBJECT
public:
	ScriptItemManager(QSharedPointer<EditorState> editorState, QGraphicsItem *scriptsParent, QSharedPointer<Asset> asset);
	virtual ~ScriptItemManager(){};
	virtual void setScriptBoundingRect(QRectF rect);
private:
	void updateScriptItemDimensions();
	enum SCRIPTTYPE{TYPE_FIRST = 0,ANALYSIS=0,EXPERIMENTAL,TYPE_NUM};
	enum SCRIPTTIME{TIME_FIRST = 0,ENTRY=0,FRAME,EXIT,TIME_NUM};
	QVector<QString> scriptTypePrefixs_;
	QVector<QString> scriptTimeSuffixs_;
	QVector<QColor> scriptTypeColors_;
	float minScriptItemHeight_;
	QVector<float> scriptRegionStartPos_;
	QVector<QMap<int,ScriptItem*>> scriptItems_;
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<Asset> asset_;
	QRectF scriptBoundingRect_;
	QGraphicsItem* scriptsParent_;

private slots:
	void assetEdited();
};
//! [0]

#endif
