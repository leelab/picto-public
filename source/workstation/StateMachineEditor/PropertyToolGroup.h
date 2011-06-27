#ifndef PROPERTYTOOLGROUP_H
#define PROPERTYTOOLGROUP_H
#include "ToolGroup.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;
class PropertyToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   PropertyToolGroup(QSharedPointer<EditorState> editorState,QSharedPointer<Asset> asset = QSharedPointer<Asset>(),QWidget *parent=0);
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
