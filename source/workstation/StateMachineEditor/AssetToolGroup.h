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
   AssetToolGroup(QSharedPointer<EditorState> editorState,QSharedPointer<Asset> asset = QSharedPointer<Asset>(),QWidget *parent=0);
   void setAsset(QSharedPointer<Asset> asset);
protected:
	virtual void doButtonAction(int buttonId);
	virtual void disableButtonActions();
private:
	QSharedPointer<Asset> asset_;
	struct ElemInfo{QString tag;QString type;};
	QVector<ElemInfo> elemInfo_;
};

#endif
