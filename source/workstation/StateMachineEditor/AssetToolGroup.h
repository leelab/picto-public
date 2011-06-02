#ifndef ASSETTOOLGROUP_H
#define ASSETTOOLGROUP_H
#include "ToolGroup.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;
class AssetToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   AssetToolGroup(QSharedPointer<Asset> asset = QSharedPointer<Asset>(),QWidget *parent=0);
   void setAsset(QSharedPointer<Asset> asset);
private:
	QSharedPointer<Asset> asset_;
};

#endif
