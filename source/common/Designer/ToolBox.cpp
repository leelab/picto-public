#include <QtWidgets>
#include "Toolbox.h"
//#include "AssetToolGroup.h"
//#include "PropertyToolGroup.h"
//#include "BackgroundToolGroup.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a new Toolbox.  
 *	\details editorState is used to detect when the Designer Window Asset changes and 
 *	whether the Designer is operating in Experimental or Analysis mode so that
 *	the Toolbox can update the available tools accordingly.
 *	\sa EditorState::windowAssetChanged()
 */
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

/*! \brief Updates the contents of the Toolbox so that they will be appropriate for editing the input asset.
 *	\details This function pretty much just chooses which kind of ToolGroup objects will need to be
 *	added based on the design mode (Analysis or Experimental).  It adds the ToolGroups using
 *	addToolGroup().
 */
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
		//Analysis Elements are only allowed in Machine Containers
		if(asset->inherits("Picto::MachineContainer"))
		{
			QSharedPointer<Asset> currAnalysis = editorState_->getCurrentAnalysis();
			addToolGroup(QStringList() <<"Variables","Variables",currAnalysis);
			addToolGroup(QStringList() <<"Outputs","Outputs",currAnalysis);
			addToolGroup(QStringList() <<"Logic Elements","Logic Elements",currAnalysis);
			addToolGroup(QStringList() <<"Sensors","Sensors",currAnalysis);
		}
	}

	//Restore the previous index
	if(currIndex > count())
		currIndex = 0;
	setCurrentIndex(currIndex);
}

/*! \brief Creates a ToolGroup according to the input specifications and adds it to this Toolbox.
 *	\details Currently we are only using AssetToolGroup objects.  tagFilters and windowAsset are 
 *	passed into the AssetToolGroup constructor.  groupName is added as a label to a maximizable/minimizable 
 *	bar that is used to display/hide each AssetToolGroup.
 */
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