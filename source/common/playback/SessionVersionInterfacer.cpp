#include <QWeakPointer>
#include "SessionVersionInterfacer.h"
#include "../storage/DataStore.h"
using namespace Picto;

/*! \brief Constructs a new SessionVersionInterfacer object.
 *	\details The input Experiment is the one who's tree will have its Asset IDs corrected according
 *	to the values input in the addSessionProperty(), addSessionTransition(), and addSessionElement() functions.
 */
SessionVersionInterfacer::SessionVersionInterfacer(QSharedPointer<Experiment> initializedExperiment)
:experiment_(initializedExperiment)
{
}

SessionVersionInterfacer::~SessionVersionInterfacer()
{
}

/*! \brief Adds a Property from the current Session file which will be used to correct the asset id
 *	of the corresponding Property in the Experiment handled by this SessionVersionInterfacer.
 * Returns an empty string on success and an error message on failure.
 */
QString SessionVersionInterfacer::addSessionProperty(QString name, int assetId, int parentAssetId)
{
	if(nodesById_.contains(assetId))
		return QString("More than one asset has the same id:%1").arg(assetId);
	nodesById_[assetId] = QSharedPointer<SVIAssetNode>(new SVIAssetNode(name,assetId,parentAssetId,""));
	return "";
}

/*! \brief Adds a Transition from the current Session file which will be used to correct the asset id
 *	of the corresponding Transition in the Experiment handled by this SessionVersionInterfacer.
 * Returns an empty string on success and an error message on failure.
 */
QString SessionVersionInterfacer::addSessionTransition(QString source, QString sourceResult, int assetId, int parentAssetId)
{
	if(nodesById_.contains(assetId))
		return QString("More than one asset has the same id:%1").arg(assetId);
	QSharedPointer<SVIAssetNode> newNode = QSharedPointer<SVIAssetNode>(new SVIAssetNode("",assetId,parentAssetId,""));
	newNode->transSource = source;
	newNode->transResult = sourceResult;
	newNode->isTransition = true;
	nodesById_[assetId] = newNode;
	return "";
}

/*! \brief Adds an Experimental Element from the current Session file which will be used to correct the asset id
 *	of the corresponding Experimental Element in the Experiment handled by this SessionVersionInterfacer.
 *	\details An Experimental Element is anything that is not a Property or a Transition but is part of the Experiment
 *	design, ie. StateMachineElements, Variables, Parameters, etc.
 * Returns an empty string on success and an error message on failure.
 */
QString SessionVersionInterfacer::addSessionElement(int assetId, QString path)
{
	if(nodesById_.contains(assetId))
		return QString("More than one asset has the same id:%1").arg(assetId);
	QString name = "";
	if(path != "")
		name = path.split("::").last();
	QSharedPointer<SVIAssetNode> newNode = QSharedPointer<SVIAssetNode>(new SVIAssetNode(name,assetId,-1,path));
	if(path != "")
		nodesByPath_[path] = newNode;
	nodesById_[assetId] = newNode;
	return "";
}

/*! \brief Updates all of the Asset ID values in the Experiment handled by this SessionVersionInterfacer to be equal
 *	to the Asset IDs of assets input in the addSessionProperty(), addSessionTransition(), and addSessionElement() functions.
 *	\details Any Session Assets input through the various functions that can't be matched up to an Experimental Asset 
 *	will be considered to be Obsolete Assets, a list of their Asset IDs can be returned from getObsoleteAssets()
 *	so that changes to their values or their descendnants Property values can be ignored.
 * Returns an empty string on success and an error message on failure.
 */
QString SessionVersionInterfacer::updateSessionConfigFromSessionAssets()
{
	////////////////////////////////////////////////
	//Build children arrays into sessionAssetNodes
	////////////////////////////////////////////////
	foreach(QSharedPointer<SVIAssetNode> node,nodesById_)
	{	//Clear all child arrays
		node->children.clear();
	}
	//Build child arrays and keep a list of task nodes and nameless nodes while we're at it.
	QList<QSharedPointer<SVIAssetNode>> taskNodes;
	QList<QSharedPointer<SVIAssetNode>> pathlessNodes;
	foreach(QSharedPointer<SVIAssetNode> node,nodesById_)
	{
		if(node->parent <= 0)
		{	//If node doesn't have parent id, use path value to load node->parent value
			if(node->path == "")
			{
				pathlessNodes.append(node);
				continue;
			}
			QStringList pathList = node->path.split("::");
			pathList.removeLast();
			QString parentPath = pathList.join("::");
			if(parentPath == "")
			{
				taskNodes.append(node);
				continue;
			}
			if(!nodesByPath_.contains(parentPath))
			{
				return QString("Node with name:%1 and assetId:%2 has an invalid path entry of:%3").arg(node->name).arg(node->assetId).arg(node->path);
			}
			node->parent = nodesByPath_.value(parentPath)->assetId;
		}
		if(!nodesById_.contains(node->parent))
		{
			return QString("Node with name:%1 and assetId:%2 has an invalid parent entry of:%3").arg(node->name).arg(node->assetId).arg(node->parent);
		}
		//Update parent asset's child list
		nodesById_[node->parent]->children.append(node->assetId);
	}

	////////////////////////////////////////////////
	//Find experiment node and set task nodes as its children
	////////////////////////////////////////////////
	//Experiment node is pathless and has a child named Syntax Version or XGain or YGain or XOffset or YOffset
	int experimentIndex = -1;
	foreach(QSharedPointer<SVIAssetNode> node,pathlessNodes)
	{
		foreach(int child,node->children)
		{
			if	(	
						nodesById_[child]->name == "SyntaxVersion"	
					||	nodesById_[child]->name == "XGain"
					||	nodesById_[child]->name == "YGain"
					||	nodesById_[child]->name == "XOffset"
					||	nodesById_[child]->name == "YOffset"
				)
			{
				experimentIndex = node->assetId;
				break;
			}
		}
		if(experimentIndex >= 0)
			break;
	}
	if(experimentIndex < 0)
	{
		return QString("Could not find experiment asset");
	}
	foreach(QSharedPointer<SVIAssetNode> node,taskNodes)
	{
		node->parent = experimentIndex;
		nodesById_[experimentIndex]->children.append(node->assetId);
	}

	////////////////////////////////////////////////
	//Set new asset ids to all assets
	////////////////////////////////////////////////
	//Start by setting all asset ids back to zero, any that are not reset with
	//values from the session will be fixed by the DesignConfig later on.
	QSharedPointer<DesignConfig> designConfig = experiment_->getDesignConfig();
	if(!designConfig)
		return "Experiment had no associated Design Config object";
	QList<QWeakPointer<Asset>> assets = designConfig->getAssets();
	QSharedPointer<Asset> strongAsset;
	foreach(QWeakPointer<Asset> weakAsset,assets)
	{
		strongAsset = weakAsset.toStrongRef();
		if(strongAsset)
		{
			//qDebug(QString("A:AssetId:%1, Name:%2").arg(strongAsset->getAssetId()).arg(strongAsset->getName()).toLatin1());
			strongAsset->setAssetId(0);
		}
		else
			Q_ASSERT(false);//This should never happen, but... just in case.
	}
	QString r = recurseResetAssetIds(experiment_,nodesById_[experimentIndex]);

	////////////////////////////////////////////////
	//Reset Design Config, and add all assets again
	////////////////////////////////////////////////
	designConfig->reset();
	foreach(QWeakPointer<Asset> weakAsset,assets)
	{
		strongAsset = weakAsset.toStrongRef();
		if(strongAsset)
		{
			//qDebug(QString("B:AssetId:%1, Name:%2").arg(strongAsset->getAssetId()).arg(strongAsset->getName()).toLatin1());
			designConfig->addManagedAsset(strongAsset);
		}
	}
	designConfig->disallowIdDuplication();
	designConfig->fixDuplicatedAssetIds();
	
assets = designConfig->getAssets();
foreach(QWeakPointer<Asset> weakAsset,assets)
{
	strongAsset = weakAsset.toStrongRef();
	//if(strongAsset)
	//{
	//	qDebug(QString("C:AssetId:%1, Name:%2").arg(strongAsset->getAssetId()).arg(strongAsset->getName()).toLatin1());
	//}
}

	return "";
}

/*! \brief Returns a lookup table of AssetIDs from the Session.
 *	\details Assets that were added through the addSessionProperty(), addSessionTransition(), and addSessionElement() functions
 *	that could not be identified in the Experiment handled by this SessionVersionInterfacer must have been removed due to an 
 *	Experiment version upgrade.  Use this lookup table to identify these Assets so that changes in their values can be ignored.
 */
QHash<int,bool> SessionVersionInterfacer::getObsoleteAssets()
{
	return obsoleteAssets_;
}

/*! \brief A recursive function used to reset the Asset ID the input asset from that of the input node and do
 *	the same for all child Assets of the input asset and node.
 *	\details If no matching Asset is found for any SVIAssetNode, that SVIAssetNode's asset ID along with any
 *	Asset IDs of its children are added to the obsoleteAssets_ table that can be accessed with getObsoleteAssets().
 *	recurseAddObsoleteAssets() is used for this purpose.
 *	Returns an empty string on success and an error message on failure.
 */
QString SessionVersionInterfacer::recurseResetAssetIds(QSharedPointer<Asset> asset,QSharedPointer<SVIAssetNode> node)
{
	asset->setAssetId(node->assetId);
	if(!asset->inherits("Picto::DataStore"))
		return "";
	QSharedPointer<DataStore> dataStore = asset.staticCast<DataStore>();
	QStringList childTags = dataStore->getDefinedChildTags();
	QList<QSharedPointer<Asset>> childAssets;
	foreach(QString childTag,childTags)
	{
		childAssets << dataStore->getGeneratedChildren(childTag);
	}
	foreach(int childId,node->children)
	{
		QSharedPointer<SVIAssetNode> childNode = nodesById_[childId];
		QSharedPointer<Asset> matchingNode;
		foreach(QSharedPointer<Asset> child,childAssets)
		{
			if(childNode->isTransition != child->inherits("Picto::Transition"))
				continue;
			if(childNode->isTransition)
			{
				QSharedPointer<Transition> transChild = child.staticCast<Transition>();
				if(		(childNode->transSource == transChild->getSource())
					&&	(childNode->transResult == transChild->getSourceResult())
					)
				{
					matchingNode = child;
					break;
				}
			}
			else if(childNode->name == child->getName())
			{
				matchingNode = child;
				break;
			}
		}
		if(matchingNode)
		{
			QString r = recurseResetAssetIds(matchingNode,childNode);
			if(r != "")
				return r;
		}
		else
		{
		//If no matching node was found, the asset must either be one of the types not
		//recorded in the session (UI stuff) or come from an updated version of an asset
		//used in the experiment.  Either way add it to an obsolete assets list that can
		//be used to filter out changes.
			recurseAddObsoleteAssets(childNode->assetId);
		}
	}
	return "";
}

/*! \brief Adds the input assetId to the obsoleteAssets_ lookup.  Then recurses to add all descendant Asset IDs to the
 *	lookup table as well.
 */
void SessionVersionInterfacer::recurseAddObsoleteAssets(int assetId)
{
	obsoleteAssets_[assetId] = true;
	//qDebug(QString("No Matching Asset found for Node with AssetId:%1, Name:%2").arg(assetId).arg(nodesById_[assetId]->name).toLatin1());
	foreach(int childId,nodesById_[assetId]->children)
	{
		recurseAddObsoleteAssets(childId);
	}
}