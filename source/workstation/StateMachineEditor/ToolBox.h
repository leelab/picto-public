#ifndef TOOLBOX_H
#define TOOLBOX_H
#include <QToolBox>
#include "ToolGroup.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
#include <QString>
using namespace Picto;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QButtonGroup;
class QToolButton;
class QAbstractButton;
QT_END_NAMESPACE

//! [0]
class Toolbox : public QToolBox
{
    Q_OBJECT

public:
   Toolbox(QWidget *parent=0);
   void setAsset(QSharedPointer<Asset> asset);
   void resize();
   QString getSelectedButton();
signals:
   void insertionItemSelected(QString itemName);
private:
	ToolGroup* assetTools_;
	ToolGroup* backgroundTools_;
};
//! [0]

#endif
