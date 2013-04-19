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
	ScriptItemManager(QSharedPointer<EditorState> editorState, QGraphicsItem *scriptsParent, QSharedPointer<Asset> asset, bool horizontal);
	virtual ~ScriptItemManager(){};
	virtual void setScriptBoundingRect(QRectF rect);
private:
	void updateScriptItemDimensions();
	enum SCRIPTTYPE{ANALYSISENTRY=0,ENTRY,FRAME,ANALYSISFRAME,EXIT,ANALYSISEXIT,TYPE_NUM};
	QVector<QString> orderedScriptNames_;
	QVector<QColor> scriptTypeColors_;
	float minScriptItemHeight_;
	QVector<float> scriptRegionStartPos_;
	QMap<int,ScriptItem*> scriptItems_;
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<Asset> asset_;
	QRectF scriptBoundingRect_;
	bool horizontal_;
	QGraphicsItem* scriptsParent_;

private slots:
	void updateScriptItems();
	void performAnalysisScriptContainerOps(QSharedPointer<Asset> assetChild);
};
//! [0]

#endif
