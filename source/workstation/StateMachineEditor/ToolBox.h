#ifndef TOOLBOX_H
#define TOOLBOX_H
#include <QToolBox>
#include "ToolGroup.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
#include <QString>
#include <QList>
#include "EditorState.h"
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
   Toolbox(QSharedPointer<EditorState> editorState,QWidget *parent=0);

public slots:
   void setAsset(QSharedPointer<Asset> asset);

private:
	QList<ToolGroup*> toolGroups_;
	QSharedPointer<EditorState> editorState_;
};
//! [0]

#endif
