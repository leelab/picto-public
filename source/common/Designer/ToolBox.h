#ifndef TOOLBOX_H
#define TOOLBOX_H
#include <QToolBox>
#include "AssetToolGroup.h"
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

/*! \brief A Toolbox widget containing groups of buttons that have some affect on activity in the designer.
 *	\details The Toolbox is essentially a collection of ToolGroup objects, each of which appears in its own
 *	maximizable/minimizable sub-section of a widget.  When we first created the Toolbox, we supported multiple
 *	types of ToolGroup classes for creating ToolGroup objects.  These could still be easily supported, but right 
 *	now we are only creating ToolGroups from the AssetToolGroup since the other ToolGroups were either not particularly
 *	useful or no longer conceptually meaningful (ie. PropertyToolGroup).
 *
 *	Most of the more complicated work needed by the Toolbox is happening in the ToolGroup and its sub-classes.  The 
 *	Toolbox itself pretty much just collects ToolGroups, and lays them out in a Toolbox framework.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class Toolbox : public QToolBox
{
    Q_OBJECT

public:
	Toolbox(QSharedPointer<EditorState> editorState,QWidget *parent=0);
	virtual ~Toolbox(){};

public slots:
   void setAsset(QSharedPointer<Asset> asset);

private:
	void addToolGroup(QStringList tagFilters,QString groupName,QSharedPointer<Asset> windowAsset);
	QList<AssetToolGroup*> toolGroups_;
	QSharedPointer<EditorState> editorState_;
};


#endif
