#include <QtWidgets>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
#include "../parameter/AnalysisScriptHolder.h"
#include "../property/Property.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new PropertyBrowser.
 *	\details The editorState is used to detect what exactly is being selected in the Designer canvas
 *	and select the appropriate Property Widgets for display accordingly.
 */
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	QWidget(parent),
	editorState_(editorState)
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(arrowPortSelected(QSharedPointer<Asset>)),
        this, SLOT(arrowPortSelected(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(releaseEditorMemory()), this, SLOT(clearAssets()));

	propGroupWidget_ = new PropertyGroupWidget(false,editorState);
	QVBoxLayout* myLayout = new QVBoxLayout();
	myLayout->addWidget(propGroupWidget_);
	setLayout(myLayout);
	myLayout->setContentsMargins(QMargins(0,0,0,0));

	//Set up script name order
	orderedScriptNames_.append("AnalysisEntryScript");
	orderedScriptNames_.append("EntryScript");
	orderedScriptNames_.append("FrameScript");
	orderedScriptNames_.append("AnalysisFrameScript");
	orderedScriptNames_.append("ExitScript");
	orderedScriptNames_.append("AnalysisExitScript");
	//Generate lookup table of script names
	foreach(QString name,orderedScriptNames_)
		scriptNamesLookup_[name] = true;
	connect(propGroupWidget_, SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyEdited(QSharedPointer<Property>,QVariant)));
}

/*! \brief Called when a new Asset is selected in the Designer to find all of its non-script Properties and add them
 *	 to the PropertyGroupWidget for display.
 */
void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
	propGroupWidget_->clear();
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	QStringList orderedProps = dataStore->getOrderedPropertyList();
	QVector<QSharedPointer<Property>> propVec;
	foreach(QString propTag,orderedProps)
	{
		foreach(QSharedPointer<Property> prop,properties.value(propTag))
		{
			if(prop->isVisible() && !scriptNamesLookup_.contains(propTag))
				propVec << prop;
		}
	}
	//propGroupWidget_->clear();
	propGroupWidget_->addProperties(dataStore->getName(),propVec);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

/*! \brief Called whenever a property handled by this PropertyBrowser's PropertyGroupWidget is edited to tell the
 *	EditorState to make the laset edit undoable.
 */
void PropertyBrowser::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	//if (!scriptNamesLookup_.contains(prop->getName()))
	if (prop->type() != QVariant::String)
	{
		editorState_->setLastActionUndoable();
	}
}

/*! \brief Called when an ArrowPortItem is selected in the Designer to find the appropriate script Properties and add them
 *	 to the PropertyGroupWidget for display.
 *	\details The displayed script properties depend on whether the selected Arrow port was a Start bar or a result bar.
 *	They also depend on whether there is an active analysis.
 *	\note If there is an active Analysis, this function also checks to see if the selected Asset contains an
 *	AnalysisScriptHolder.  If it doesn't, it adds one to selected Asset so that it can show the Analysis Scripts and the
 *	user will be able to edit them.
 *	This is important.  It is somewhat counter intuitive that the PropertyBrowser would be responsible for creating
 *	AnalysisScriptHolders.  There are many ways that we could have done this, and each has its merits.  This is the one
 *	that we chose, but it might be worth considering just automatically adding AnalysisScriptHolders to all
 *	StateMachineElement objects as soon as a new Analysis is created.
 */
void PropertyBrowser::arrowPortSelected(QSharedPointer<Asset> asset)
{
	//Only show scripts
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	//If this object doesn't have an AnalysisScriptHolder and their is an active analysis, add one now
	QSharedPointer<Analysis> activeAnalysis = editorState_->getCurrentAnalysis();
	if(activeAnalysis && asset->inherits("Picto::StateMachineElement") && asset.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptHolder").isEmpty())
	{
		//create the AnalysisScriptHolder, put it in the analysis 
		//and link it to the object
		QSharedPointer<AnalysisScriptHolder> newScriptContainer = editorState_->getCurrentAnalysis()->createChildAsset("AnalysisScriptHolder",QString(),QString()).staticCast<AnalysisScriptHolder>();
		//Add entry and exit scripts
		newScriptContainer->createChildAsset("AnalysisEntryScript",QString(),QString());
		newScriptContainer->createChildAsset("AnalysisExitScript",QString(),QString());
		if(asset->inherits("Picto::State"))
		{//Its a state, add a frame script
			newScriptContainer->createChildAsset("AnalysisFrameScript",QString(),QString());
		}
		//Add this newScriptContainer to the asset
		asset.staticCast<DataStore>()->AddAssociateChild(newScriptContainer->getAssociateId(),"AnalysisScriptHolder",newScriptContainer);
	}
	QSharedPointer<AnalysisScriptHolder> scriptContainer;
	if(activeAnalysis && asset->inherits("Picto::StateMachineElement"))
	{
		scriptContainer = asset.staticCast<DataStore>()->getAssociateChildren(activeAnalysis->getAssociateId(),"AnalysisScriptHolder").first().staticCast<AnalysisScriptHolder>();
	}
	QSharedPointer<PropertyContainer> propContainer;
	QVector<QSharedPointer<Property>> propVec;
	foreach(QString propTag,orderedScriptNames_)
	{
		//Find out if this propTag is for an analysis script
		bool isAnalysisScript = propTag.left(1).toLower() == "a";
		//If this propTag is not for an analysis script, get the property container from the asset.
		if(!isAnalysisScript)
		{
			propContainer = asset.staticCast<DataStore>()->getPropertyContainer();
		}
		else
		{
			//If there's an active analysis, get the propContainer from the AnalysisScriptHolder, otherwise leave it empty
			if(activeAnalysis && scriptContainer)
			{
				//The propTag is for an analysis script, set the property container from the AnalysisScriptHolder
				propContainer = scriptContainer.staticCast<DataStore>()->getPropertyContainer();
			}
		}

		//Get the property and add it to the PropVec if its visible.
		if(propContainer)
		{
			QSharedPointer<Property> prop = propContainer->getProperty(propTag);
			if(prop && prop->isVisible())
				propVec << prop;
		}
	}
	propGroupWidget_->clear();
	propGroupWidget_->addProperties(dataStore->getName(),propVec);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

//! Called to release Designer assets so it may be cleaned up.  Will need to be reinitialized before use.
void PropertyBrowser::clearAssets()
{
	propGroupWidget_->clear();
}