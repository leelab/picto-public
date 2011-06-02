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
	WireableItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	QSharedPointer<Asset> getAsset(){return asset_;};
	void addArrowSource(QString name);
	void enableArrowDest();
	QList<DiagramItem*> getArrowSources();
	DiagramItem* getArrowDest();
	virtual void removeDependantGraphics();
protected:
	virtual void updateDependantGraphics(){};
private:
	QSharedPointer<Asset> asset_;
	QList<DiagramItem*> arrowSources_;
	DiagramItem* arrowDest_;
	int lastSourcePos_;

};
//! [0]

#endif
