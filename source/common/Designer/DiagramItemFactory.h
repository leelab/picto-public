#ifndef DIAGRAMITEMFACTORY_H
#define DIAGRAMITEMFACTORY_H

#include "diagramitem.h"
#include "../../common/storage/asset.h"
#include <QMap>
using namespace Picto;

struct IconDef;
/*!	\brief Handles creation of DiagramItems for Design Assets in the Designer.
 *	\details Implements a create() function that takes in an Asset pointer and returns a DiagramItem 
 *	of the appropriate type with the approperiate size and icon.  Icons are compiled into the 
 *	Pictolib library.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class DiagramItemFactory
{
public:
	DiagramItemFactory(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QGraphicsScene *scene);
	virtual ~DiagramItemFactory(){};
	DiagramItem* create(QSharedPointer<Asset> asset);
	static QIcon getIcon(QString uITemplate);
	static void addIconDefinition(QString assetType, QString fileName,float width, float height);
private:
	static IconDef getIconDef(QSharedPointer<Asset> asset);
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


#endif
