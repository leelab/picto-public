#include <QWeakPointer>
#include "SessionVersionInterfacer.h"
#include "../storage/DataStore.h"
using namespace Picto;

SessionVersionInterfacer::SessionVersionInterfacer(QSharedPointer<Experiment> initializedExperiment)
:experiment_(initializedExperiment)
{
}

SessionVersionInterfacer::~SessionVersionInterfacer()
{
}

QString SessionVersionInterfacer::addSessionProperty(QString name, int assetId, int parentAssetId)
{
	if(nodesById_.contains(assetId))
		return QString("More than one asset has the same id:%1").arg(assetId);
	nodesById_[assetId] = QSharedPointer<SVIAssetNode>(new SVIAssetNode(name,assetId,parentAssetId,""));
	return "";
}

QString SessionVersionInterfacer::addSessionTransition(QString source, QString sourceResult, int assetId, int parentAssetId)
{
	if(assetId == 4623)
	{
		int i=0;
		i++;
	}
	if(nodesById_.contains(assetId))
		return QString("More than one asset has the same id:%1").arg(assetId);
	QSharedPointer<SVIAssetNode> newNode = QSharedPointer<SVIAssetNode>(new SVIAssetNode("",assetId,parentAssetId,""));
	newNode->transSource = source;
	newNode->transResult = sourceResult;
	newNode->isTransition = true;
	nodesById_[assetId] = newNode;
	return "";
}

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
	//values from the session will be fixed by the experimentConfig later on.
	QSharedPointer<ExperimentConfig> expConfig = experiment_->getExperimentConfig();
	if(!expConfig)
		return "Experiment had no associated Experiment Config object";
	QList<QWeakPointer<Asset>> assets = expConfig->getAssets();
	QSharedPointer<Asset> strongAsset;
	foreach(QWeakPointer<Asset> weakAsset,assets)
	{
		strongAsset = weakAsset.toStrongRef();
		if(strongAsset)
		{
			qDebug(QString("A:AssetId:%1, Name:%2").arg(strongAsset->getAssetId()).arg(strongAsset->getName()).toAscii());
			strongAsset->setAssetId(0);
		}
		else
			Q_ASSERT(false);//This should never happen, but... just in case.
	}
	QString r = recurseResetAssetIds(experiment_,nodesById_[experimentIndex]);

	////////////////////////////////////////////////
	//Reset experiment config, and add all assets again
	////////////////////////////////////////////////
	expConfig->reset();
	foreach(QWeakPointer<Asset> weakAsset,assets)
	{
		strongAsset = weakAsset.toStrongRef();
		if(strongAsset)
		{
			qDebug(QString("B:AssetId:%1, Name:%2").arg(strongAsset->getAssetId()).arg(strongAsset->getName()).toAscii());
			expConfig->addManagedAsset(strongAsset);
		}
	}
	expConfig->disallowIdDuplication();
	expConfig->fixDuplicatedAssetIds();
	
assets = expConfig->getAssets();
foreach(QWeakPointer<Asset> weakAsset,assets)
{
	strongAsset = weakAsset.toStrongRef();
	if(strongAsset)
	{
		qDebug(QString("C:AssetId:%1, Name:%2").arg(strongAsset->getAssetId()).arg(strongAsset->getName()).toAscii());
	}
}

	return "";
}

QHash<int,bool> SessionVersionInterfacer::getObsoleteAssets()
{
	return obsoleteAssets_;
}

QString SessionVersionInterfacer::recurseResetAssetIds(QSharedPointer<Asset> asset,QSharedPointer<SVIAssetNode> node)
{
	if(node->assetId == 4623)
	{
		QSharedPointer<Transition> trans = asset.staticCast<Transition>();
		QString src = trans->getSource();
		QString srcRes = trans->getSourceResult();
		QString dest = trans->getDestination();
		int i=0;
		i++;
	}
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

void SessionVersionInterfacer::recurseAddObsoleteAssets(int assetId)
{
	obsoleteAssets_[assetId] = true;
	qDebug(QString("No Matching Asset found for Node with AssetId:%1, Name:%2").arg(assetId).arg(nodesById_[assetId]->name).toAscii());
	foreach(int childId,nodesById_[assetId]->children)
	{
		recurseAddObsoleteAssets(childId);
	}
}