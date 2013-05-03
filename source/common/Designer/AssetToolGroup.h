#ifndef ASSETTOOLGROUP_H
#define ASSETTOOLGROUP_H
#include <QSharedPointer>
#include "ToolGroup.h"
#include "../storage/AssetFactory.h"
#include "../storage/asset.h"
using namespace Picto;
class AssetToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   AssetToolGroup(QSharedPointer<EditorState> editorState,QStringList assetTagFilters,QSharedPointer<Asset> asset = QSharedPointer<Asset>(),QWidget *parent=0);
   virtual ~AssetToolGroup(){};
   void setAsset(QSharedPointer<Asset> asset);
   bool isEmpty();
protected:
	virtual void doButtonAction(int buttonId);
	virtual void disableButtonActions();
	virtual bool isEnabled(int buttonId);
private:
	QSharedPointer<Asset> asset_;
	QStringList assetTagFilters_;
	struct ElemInfo{QString tag;QString type;QSharedPointer<AssetFactory> assetFactory;};
	QVector<ElemInfo> elemInfo_;
};

#endif
