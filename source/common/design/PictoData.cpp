#include "PictoData.h"
#include "../parameter/Analysis.h"
#include "../statemachine/UIData.h"
#include "../common/storage/ObsoleteAsset.h"
#include "../task/Task.h"
#include "../common/memleakdetect.h"
using namespace Picto;

PictoData::PictoData()
{
	//By default, a PictoData element's name is "Untitled" and "SyntaxVersion" is empty.
	AddDefinableProperty("Name","Untitled");
	AddDefinableProperty("SyntaxVersion","");
	AddDefinableObjectFactory("Experiment",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(Experiment::Create))));
	AddDefinableObjectFactory("Analysis",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Analysis::Create))));
	AddDefinableObjectFactory("UIData",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(UIData::Create))));
	
	//Add Obsolete Asset Factories - These used to be part of the Picto tree but were deprecated and are now being removed
	AddDefinableObjectFactory("AnalysisContainer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableObjectFactory("StateMachineEditorData",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

void PictoData::postDeserialize()
{
	//If this PictoData has any StateMachineEditorData child assets, this object should be set as edited.
	//  ideally, the ObsoleteAsset child would just set itself deleted and that would
	//  propogate its edited state to here, the ObsoleteAsset can't though because 
	//  the latest it can do that is in postDeserialize, and this PictoData has its
	//  edited flag set back to false after that point.  This should be fixed at some point.
	QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren("StateMachineEditorData");
	if(tagChildren.size())
		emit edited();

	//Add all associate roots to their hosts (this has the affect of adding all analyses to their tasks and all
	//  UIData to their experiments and analyses.
	QList<QSharedPointer<Asset>> allAssociateRoots = getGeneratedChildren("Analysis");
	allAssociateRoots.append(getGeneratedChildren("UIData"));
	QSharedPointer<Asset> linkAsset;
	QString feedback;
	foreach(QSharedPointer<Asset> associateRoot,allAssociateRoots)
	{
		linkAsset = associateRoot.staticCast<AssociateRoot>()->getLinkableAsset();
		//If the Associate Root has nothing to link to, delete it.
		if(!linkAsset)
		{
			associateRoot->setDeleted();
			continue;
		}
		associateRoot.staticCast<AssociateRoot>()->LinkToAsset(linkAsset,feedback);
	}

	//Now that the entire tree is set up, and the saved associate roots are attached to their linked elements based on
	//  saved asset ids, make sure that all assets have unique IDs and the DesignConfig object is tracking them.  This way,
	//  if an assetId needs to be changed for some reason, the changed id will propegate to any linked associates, and
	//  resulting hostId changes will propogate to linked AssociateRoots as well.
	//Since Associates to AssociateHost linking does not use the design config, it is okay that we disallowIdDuplication
	//  after connecting Associates.
	designConfig_->disallowIdDuplication();

	//Now call inherited postDeserialize which will actually call fixDuplicatedAssetIds on the designConfig
	DataStore::postDeserialize();

	QList<QSharedPointer<Asset>> assocRootHosts = getGeneratedChildren("Experiment");
	assocRootHosts.append(getGeneratedChildren("Analysis"));

	AssociateRootHost* assocRootHost;
	foreach(QSharedPointer<Asset> associateRootHostAsset,assocRootHosts)
	{
		//If the host has no UIData AssociateRoot attached
		assocRootHost = dynamic_cast<AssociateRootHost*>(associateRootHostAsset.data());
		Q_ASSERT(assocRootHost);
		if(!assocRootHost->getAssociateRoot("UIData"))
		{
			//Create a new UIData object and attach it to this host.
			QUuid hostId = assocRootHost->getHostId();
			QSharedPointer<Asset> newUIData = createChildAsset("UIData",QString(),QString());
			QString feedback;
			newUIData.staticCast<AssociateRoot>()->LinkToAsset(associateRootHostAsset,feedback);
		}
	}

	//We handle version upgrading after attaching AssociateRoots so that those Associate roots
	//  can be modified, if necessary, as part of the upgrade process and so that changes to their
	//  hosts will not cause them to get disconnected (hostId change will propegate to associateRoot).
	QString syntaxVer = propertyContainer_->getPropertyValue("SyntaxVersion").toString();
	if(syntaxVer != DESIGNSYNTAXVERSION)
	{
		designConfig_->setDeserializedVersion(syntaxVer);
		upgradeVersion(syntaxVer);
		propertyContainer_->setPropertyValue("SyntaxVersion",DESIGNSYNTAXVERSION);
	}
	propertyContainer_->getProperty("SyntaxVersion")->setVisible(false);
}

/*! \brief Constructs a shared pointer to a new PictoData object and returns it.
 *	\details Whereas in all other node of the Picto design tree, the parent sets the childs DesignConfig pointer to be the
 *	same as its own, since we are at the top of the tree, we create the DesignConfig object here and set it to our own
 *	pointer.
 */
QSharedPointer<Asset> PictoData::Create()
{
	QSharedPointer<Asset> newPictoData(new PictoData());
	newPictoData->setSelfPtr(newPictoData);
	newPictoData->setDesignConfig(QSharedPointer<DesignConfig>(new DesignConfig()));
	return newPictoData;
}

/*! \brief Returns the Experiment child of this object.
 *	\details Every PictoData object has one and only one Experiment child.
 */
QSharedPointer<Experiment> PictoData::getExperiment()
{
	QList<QSharedPointer<Asset>> expList = getGeneratedChildren("Experiment");
	return expList.first().staticCast<Experiment>();
}

/*! \brief Returns a list of pointers to Analysis children.
 *	\details PictoData objects can have any number (including zero) of Analysis children.
 */
QList<QSharedPointer<Analysis>> PictoData::getAnalyses()
{
	QList<QSharedPointer<Analysis>> returnVal;
	foreach(QSharedPointer<Asset> asset, getGeneratedChildren("Analysis"))
	{
		returnVal.append(asset.staticCast<Analysis>());
	}
	return returnVal;
}

bool PictoData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}