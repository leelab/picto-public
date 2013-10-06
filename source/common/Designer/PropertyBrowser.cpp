#include <QtWidgets>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
#include "../parameter/AnalysisScriptHolder.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	QWidget(parent),
	editorState_(editorState)
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(arrowPortSelected(QSharedPointer<Asset>)),
        this, SLOT(arrowPortSelected(QSharedPointer<Asset>)));

	propGroupWidget_ = new PropertyGroupWidget(false,editorState);
	QVBoxLayout* myLayout = new QVBoxLayout();
	myLayout->addWidget(propGroupWidget_,Qt::AlignTop | Qt::AlignLeft);
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

void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
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
	propGroupWidget_->clear();
	propGroupWidget_->addProperties(dataStore->getName(),propVec);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

void PropertyBrowser::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	editorState_->setLastActionUndoable();
}

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