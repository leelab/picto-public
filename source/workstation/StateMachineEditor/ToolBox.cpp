#include <QtGui>
#include "Toolbox.h"
#include "AssetToolGroup.h"
#include "PropertyToolGroup.h"
#include "BackgroundToolGroup.h"
#include "../../common/memleakdetect.h"

//! [0]
Toolbox::Toolbox(QSharedPointer<EditorState> editorState,QWidget *parent) :
	editorState_(editorState),
	QToolBox(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored));
	toolGroups_.push_back(new AssetToolGroup(editorState_));
	addItem(toolGroups_.last(),tr("Assets"));
	toolGroups_.push_back(new PropertyToolGroup(editorState_));
	addItem(toolGroups_.last(),tr("Properties"));
	toolGroups_.push_back(new BackgroundToolGroup(editorState_));
	addItem(toolGroups_.last(),tr("Backgrounds"));
	connect(editorState_.data(), SIGNAL(windowAssetChanged(QSharedPointer<Asset>)),
		this, SLOT(setAsset(QSharedPointer<Asset>)));
}

void Toolbox::setAsset(QSharedPointer<Asset> asset)
{
	//For some reason, simply changing the items in the AssetToolGroup messed up QT's
	//Auto Sizing system.  For this reason, we simply create a new AssetToolGroup each
	//time this function is called and replace the old one.
	int currIndex = currentIndex();
	removeItem(0);
	delete toolGroups_.takeFirst();
	removeItem(0);
	delete toolGroups_.takeFirst();
	toolGroups_.push_front(new PropertyToolGroup(editorState_,asset));
	insertItem(0,toolGroups_.front(),tr("Properties"));
	toolGroups_.push_front(new AssetToolGroup(editorState_,asset));
	insertItem(0,toolGroups_.front(),tr("Assets"));
	setCurrentIndex(currIndex);
}