#include <QtWidgets>
#include "Toolbox.h"
//#include "AssetToolGroup.h"
//#include "PropertyToolGroup.h"
//#include "BackgroundToolGroup.h"
#include "../../common/memleakdetect.h"

//! [0]
Toolbox::Toolbox(QSharedPointer<EditorState> editorState,QWidget *parent) :
	editorState_(editorState),
	QToolBox(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored));
	//toolGroups_.push_back(new AssetToolGroup(editorState_));
	//addItem(toolGroups_.last(),tr("Assets"));
	//toolGroups_.push_back(new PropertyToolGroup(editorState_));
	//addItem(toolGroups_.last(),tr("Properties"));
	//toolGroups_.push_back(new BackgroundToolGroup(editorState_));
	//addItem(toolGroups_.last(),tr("Backgrounds"));
	connect(editorState_.data(), SIGNAL(windowAssetChanged(QSharedPointer<Asset>)),
		this, SLOT(setAsset(QSharedPointer<Asset>)));
}

void Toolbox::setAsset(QSharedPointer<Asset> asset)
{
	//For some reason, simply changing the items in the AssetToolGroup messed up QT's
	//Auto Sizing system.  For this reason, we simply create a new AssetToolGroup each
	//time this function is called and replace the old one.
	int currIndex = currentIndex();
	if(currIndex < 0)
		currIndex = 0;
	while(count() > 0)
	{
		removeItem(0);
		delete toolGroups_.takeFirst();
	}
	//toolGroups_.push_front(new PropertyToolGroup(editorState_,asset));
	//insertItem(0,toolGroups_.front(),tr("Properties"));
	QStringList filters;
	addToolGroup(QStringList() <<"State Machine Elements","State Machine Elements",asset); 
	addToolGroup(QStringList() <<"Variables","Variables",asset);
	addToolGroup(QStringList() <<"Parameters","Parameters",asset);
	addToolGroup(QStringList() <<"Logic Elements","Logic Elements",asset);
	addToolGroup(QStringList() <<"Stimulus Elements","Stimulus Elements",asset);
	addToolGroup(QStringList() <<"Sensors","Sensors",asset);
	if(currIndex > count())
		currIndex = 0;
	setCurrentIndex(currIndex);
}

void Toolbox::addToolGroup(QStringList tagFilters,QString groupName,QSharedPointer<Asset> windowAsset)
{
	toolGroups_.push_back(new AssetToolGroup(editorState_,tagFilters,windowAsset));
	addItem(toolGroups_.back(),groupName);
	//If there is nothing in this toolgroup, get rid of it.
	if(toolGroups_.back()->isEmpty())
	{
		removeItem(count()-1);
		delete toolGroups_.takeLast();
	}
}