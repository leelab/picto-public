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
	
	//Store current index
	int currIndex = currentIndex();
	if(currIndex < 0)
		currIndex = 0;
	
	//Renive previous items
	while(count() > 0)
	{
		removeItem(0);
		delete toolGroups_.takeFirst();
	}

	//Add new toolgroups to toolbox

	//If we're working on experimental design
	if(editorState_->getCurrentAnalysis().isNull())
	{
		addToolGroup(QStringList() <<"State Machine Elements","State Machine Elements",asset); 
		addToolGroup(QStringList() <<"Variables","Variables",asset);
		addToolGroup(QStringList() <<"Parameters","Parameters",asset);
		addToolGroup(QStringList() <<"Logic Elements","Logic Elements",asset);
		addToolGroup(QStringList() <<"Stimulus Elements","Stimulus Elements",asset);
		addToolGroup(QStringList() <<"Sensors","Sensors",asset);
	}
	else //If we're working on analysis design
	{ 
		QSharedPointer<Asset> currAnalysis = editorState_->getCurrentAnalysis();
		addToolGroup(QStringList() <<"Variables","Variables",currAnalysis);
		addToolGroup(QStringList() <<"Logic Elements","Logic Elements",currAnalysis);
	}

	//Restore the previous index
	if(currIndex > count())
		currIndex = 0;
	setCurrentIndex(currIndex);
}

void Toolbox::addToolGroup(QStringList tagFilters,QString groupName,QSharedPointer<Asset> windowAsset)
{
	//If we're working on experimental design
	if(editorState_->getCurrentAnalysis().isNull())
	{
		//We want the tool group to look at the window asset to find available child assets
		toolGroups_.push_back(new AssetToolGroup(editorState_,tagFilters,windowAsset));
	}
	else //If we're working on analysis design
	{
		//We want the tool group to look at the analysis object to find available child assets
		toolGroups_.push_back(new AssetToolGroup(editorState_,tagFilters,windowAsset));
	}
	addItem(toolGroups_.back(),groupName);
	//If there is nothing in this toolgroup, get rid of it.
	if(toolGroups_.back()->isEmpty())
	{
		removeItem(count()-1);
		delete toolGroups_.takeLast();
	}
}