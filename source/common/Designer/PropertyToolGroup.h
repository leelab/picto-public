#ifndef PROPERTYTOOLGROUP_H
#define PROPERTYTOOLGROUP_H
#include "ToolGroup.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;
/*! \brief A very early class in Designer development that was going to allow generation of new properties.... until we realized
 *	that there should be a fixed number of properties for every element.
 *	\details This class is no longer used.  You may delete it if you want.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PropertyToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   PropertyToolGroup(QSharedPointer<EditorState> editorState,QSharedPointer<Asset> asset = QSharedPointer<Asset>(),QWidget *parent=0);
   virtual ~PropertyToolGroup(){};
   void setAsset(QSharedPointer<Asset> asset);
protected:
	virtual void doButtonAction(int buttonId);
	virtual void disableButtonActions();
	virtual bool isEnabled(int){return true;};
private:
	QSharedPointer<Asset> asset_;
	struct ElemInfo{QString tag;QString type;};
	QVector<ElemInfo> elemInfo_;
};

#endif
