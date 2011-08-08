#ifndef DIAGRAMITEMFACTORY_H
#define DIAGRAMITEMFACTORY_H

#include "diagramitem.h"
#include "../../common/storage/asset.h"
#include "../../common/statemachine/uiinfo.h"
#include <QMap>
using namespace Picto;

//! [0]
struct IconDef;
class DiagramItemFactory
{
public:
	DiagramItemFactory(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QGraphicsScene *scene);
	DiagramItem* create(QSharedPointer<Asset> asset);
	static QIcon getIcon(QString uITemplate);
	static void addIconDefinition(QString assetType, QString fileName,float width, float height);
private:
	static IconDef getIconDef(QSharedPointer<Asset> asset);
	static QSharedPointer<UIInfo> getUIInfo(QSharedPointer<Asset> asset);
	static QMap<QString,IconDef> iconDefs_;
	QSharedPointer<EditorState> editorState_;
	QMenu* contextMenu_;
	QGraphicsScene* scene_;
	static bool mapInitialized_;

};

struct IconDef
{
	IconDef(){};
	IconDef(QString fileName, float width, float height){fileName_=fileName;width_=width;height_=height;};
	QString fileName_;
	float width_;
	float height_;
};
//! [0]

#endif
