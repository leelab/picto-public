#ifndef WIREABLEITEM_H
#define WIREABLEITEM_H

#include "AssetItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

class Arrow;

class WireableItem : public AssetItem
{
	Q_OBJECT
public:
	WireableItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~WireableItem();
	void addArrowSource(QString name);
	void enableArrowDest();
	QList<DiagramItem*> getArrowSources();
	DiagramItem* getArrowDest();
protected:
	virtual void updateDependantGraphics(){};
private:
	QList<DiagramItem*> arrowSources_;
	DiagramItem* arrowDest_;
	int lastSourcePos_;

};
//! [0]

#endif
