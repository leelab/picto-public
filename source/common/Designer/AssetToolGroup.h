#ifndef ASSETTOOLGROUP_H
#define ASSETTOOLGROUP_H
#include <QSharedPointer>
#include "ToolGroup.h"
#include "../storage/AssetFactory.h"
#include "../storage/asset.h"
using namespace Picto;
/*! \brief A ToolGroup with buttons representing Asset objects that can be added to the Designer as children of the current Window Asset.
 *	\details In practice, this is the only ToolGroup that we are using.  When a designer wants to add a new element to the current 
 *	Window Asset, they click on that Asset's button, then click on the canvas and behind the scenes the Asset represented by the button
 *	is added as a child to the Window Asset and a corresponding DiagramItem representing that Asset is added to the canvas at the input
 *	position.
 *	Asset buttons are greyed out when the maximum number of that button's Asset have already been added to the current Window Asset.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class AssetToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   AssetToolGroup(QSharedPointer<EditorState> editorState,QStringList displayedUIGroups,QSharedPointer<Asset> asset = QSharedPointer<Asset>(),QWidget *parent=0);
   virtual ~AssetToolGroup(){};
   void setAsset(QSharedPointer<Asset> asset);
   bool isEmpty();
protected:
	virtual void doButtonAction(int buttonId);
	virtual void disableButtonActions();
	virtual bool isEnabled(int buttonId);
private:
	bool isDisplayedGroup(QString UIGroup);
	QSharedPointer<Asset> asset_;
	QHash<QString,bool> uIGroupMap_;
	struct ElemInfo{QString tag;QString type;QSharedPointer<AssetFactory> assetFactory;};
	QVector<ElemInfo> elemInfo_;
};

#endif
