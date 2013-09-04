#include "DesignConfig.h"
#include "Asset.h"
#include "../statemachine/Transition.h"
#include "../memleakdetect.h"

using namespace Picto;

DesignConfig::DesignConfig()
{
	reset();
}

void DesignConfig::reset()
{
	assetsById_.clear();
	managedElements_.clear();
	managedProperties_.clear();
	managedTransitions_.clear();
	elemInfo_.clear();
	propInfo_.clear();
	transInfo_.clear();
	assetHash_.clear();
	unsortedIdAssets_.clear();
	lastUsedId_ = 0;
	allowIdDuplication_ = true;
	analysisHash_.clear();
	frameTimerFactory_ = QSharedPointer<Controller::FrameTimerFactory>(new Controller::FrameTimerFactory());
}

bool DesignConfig::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	QSharedPointer<Asset> sAsset;
	QSharedPointer<Transition> sTAsset;
	
	xmlStreamWriter->writeStartElement("DesignConfig");

	xmlStreamWriter->writeStartElement("Properties");
	foreach(QWeakPointer<Asset> wAsset,managedProperties_)
	{
		sAsset = wAsset;
		if(!sAsset)continue;
		xmlStreamWriter->writeStartElement("Prop");
		xmlStreamWriter->writeAttribute("i",QString::number(sAsset->getAssetId()));
		xmlStreamWriter->writeAttribute("n",sAsset->getName());
		xmlStreamWriter->writeAttribute("p",
			sAsset->getParentAsset()?QString::number(sAsset->getParentAsset()->getAssetId()):0);
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();

	xmlStreamWriter->writeStartElement("Transitions");
	foreach(QWeakPointer<Asset> wAsset,managedTransitions_)
	{
		sAsset = wAsset.toStrongRef();
		if(!sAsset)continue;
		sTAsset = sAsset.staticCast<Transition>();
		xmlStreamWriter->writeStartElement("Tran");
		xmlStreamWriter->writeAttribute("i",QString::number(sTAsset->getAssetId()));
		xmlStreamWriter->writeAttribute("n",sTAsset->getName());
		xmlStreamWriter->writeAttribute("p",
			sTAsset->getParentAsset()?QString::number(sTAsset->getParentAsset()->getAssetId()):0);
		xmlStreamWriter->writeAttribute("s",sTAsset->getSource());
		xmlStreamWriter->writeAttribute("r",sTAsset->getSourceResult());
		xmlStreamWriter->writeAttribute("d",sTAsset->getDestination());
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();

	xmlStreamWriter->writeStartElement("Elements");
	foreach(QWeakPointer<Asset> wAsset,managedElements_)
	{
		sAsset = wAsset;
		if(!sAsset)continue;
		xmlStreamWriter->writeStartElement("Elem");
		xmlStreamWriter->writeAttribute("i",QString::number(sAsset->getAssetId()));
		xmlStreamWriter->writeAttribute("p",sAsset->getPath());
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();
	xmlStreamWriter->writeEndElement();
	return true;
}

bool DesignConfig::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "DesignConfig")
	{
		addError("Incorrect tag, expected <DesignConfig>");
		return false;
	}
	xmlStreamReader->readNext();	//Move past the DesignConfig starting tag
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "DesignConfig") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Properties")
		{
			xmlStreamReader->readNext();	//Move past the DesignConfig starting tag
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Properties") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				PropInfo propInf;
				if(xmlStreamReader->attributes().hasAttribute("i"))
				{
					propInf.id = xmlStreamReader->attributes().value("i").toString().toInt();
				}
				else
				{
					addError("Property missing i (index) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("n"))
				{
					propInf.name = xmlStreamReader->attributes().value("n").toString();
				}
				else
				{
					addError("Property missing n (name) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					propInf.parent = xmlStreamReader->attributes().value("p").toString().toInt();
				}
				else
				{
					addError("Property missing p (parent) attribute");
					return false;
				}
				propInfo_.append(propInf);
				xmlStreamReader->readNext();
			}
		}
		else if(name == "Transitions")
		{
			xmlStreamReader->readNext();	//Move past the Transitions starting tag
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Transitions") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				TransInfo transInf;
				if(xmlStreamReader->attributes().hasAttribute("i"))
				{
					transInf.id = xmlStreamReader->attributes().value("i").toString().toInt();
				}
				else
				{
					addError("Transition missing i (index) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("n"))
				{
					transInf.name = xmlStreamReader->attributes().value("n").toString();
				}
				else
				{
					addError("Transition missing n (name) attribute");
					return false;
				}
				
				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					transInf.parent = xmlStreamReader->attributes().value("p").toString().toInt();
				}
				else
				{
					addError("Transition missing p (parent) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("s"))
				{
					transInf.source = xmlStreamReader->attributes().value("s").toString();
				}
				else
				{
					addError("Transition missing s (source) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("r"))
				{
					transInf.result = xmlStreamReader->attributes().value("r").toString();
				}
				else
				{
					addError("Transition missing r (source result) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("d"))
				{
					transInf.dest = xmlStreamReader->attributes().value("d").toString();
				}
				else
				{
					addError("Transition missing d (destination) attribute");
					return false;
				}
				transInfo_.append(transInf);
				xmlStreamReader->readNext();
			}
		}
		else if(name == "Elements")
		{
			xmlStreamReader->readNext();	//Move past the Elements starting tag
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Elements") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				AssetInfo elemInf;
				if(xmlStreamReader->attributes().hasAttribute("i"))
				{
					elemInf.id = xmlStreamReader->attributes().value("i").toString().toInt();
				}
				else
				{
					addError("Element missing i (index) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					elemInf.path = xmlStreamReader->attributes().value("p").toString();
				}
				else
				{
					addError("Element missing p (path) attribute");
					return false;
				}
				elemInfo_.append(elemInf);
				xmlStreamReader->readNext();
			}
		}
		else
		{
			addError("Invalid Tag: " + name.toLatin1());
			return false;
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void DesignConfig::addError(QString errorMessage)
{
	addErrorToList(errorMessage + "\n");
}

void DesignConfig::disallowIdDuplication()
{
	allowIdDuplication_ = false;
}

int DesignConfig::getNewDataId()
{
	++lastUsedId_;
	//Increment the id until you find an unused one
	while(assetsById_.contains(lastUsedId_))
	{
		lastUsedId_++;
	}
	return lastUsedId_;
}

void DesignConfig::addManagedAsset(QSharedPointer<Asset> asset)
{
	QWeakPointer<Asset> wAsset(asset);
	if(assetHash_.contains(asset.data()))
		return;
	assetHash_[asset.data()] = wAsset;
	if(asset->inherits("Picto::Transition"))
	{
		managedTransitions_.append(wAsset);
	}
	else if(asset->inherits("Picto::Property"))
	{
		managedProperties_.append(wAsset);
	}
	else
	{
		managedElements_.append(wAsset);
	}
	unsortedIdAssets_.append(wAsset);
}

void DesignConfig::fixDuplicatedAssetIds()
{
	if(allowIdDuplication_)
		return;
	QSharedPointer<Asset> sAsset;
	int id;
	QList<QSharedPointer<Asset>> fixList;
	
	while(unsortedIdAssets_.size())
	{
		QWeakPointer<Asset> wAsset = unsortedIdAssets_.takeFirst();
		sAsset = QSharedPointer<Asset>(wAsset);
		if(!sAsset)
			continue;	//This asset was already deleted
		id = sAsset->getAssetId();
		if(id == 0 || assetsById_.contains(id))
		{
			fixList.append(sAsset);
			continue;
		}
		assetsById_[id] = wAsset;		//Add the asset to the id map with its id.
	}
	while(fixList.size())
	{
		sAsset = fixList.takeFirst();
		id = getNewDataId();		//Get a new asset id and set it to the asset.
		sAsset->setAssetId(id);
		assetsById_[id] = QWeakPointer<Asset>(sAsset);		//Add the asset to the id map with its id.
	}
}

QSharedPointer<Asset> DesignConfig::getAsset(int id)
{
	QSharedPointer<Asset> returnVal;
	if(assetsById_.contains(id))
		returnVal = assetsById_[id];
	return returnVal;
}

QList<QWeakPointer<Asset>> DesignConfig::getAssets()
{
	return assetsById_.values();
}

void DesignConfig::setActiveAnalysisIds(QList<QUuid> analysisList)
{
	analysisHash_.clear();
	foreach(QUuid uuid,analysisList)
	{
		analysisHash_[uuid] = true;
	}
	emit activeAnalysisIdsChanged();
}

QList<QUuid> DesignConfig::getActiveAnalysisIds()
{
	return analysisHash_.keys();
}

bool DesignConfig::isAnalysisActive(QUuid analysisId)
{
	return analysisHash_.contains(analysisId);
}

void DesignConfig::markRunStart(QString runName)
{
	latestRunId_ = QUuid::createUuid();
	latestRunName_ = runName;
	emit runStarted(latestRunId_);
}

void DesignConfig::markRunEnd()
{
	emit runEnded();
}

QUuid DesignConfig::getLatestRunId()
{
	return latestRunId_;
}

QString DesignConfig::getLatestRunName()
{
	return latestRunName_;
}

QSharedPointer<Controller::FrameTimerFactory> DesignConfig::getFrameTimerFactory()
{
	return frameTimerFactory_;
}

//Analysis Data Readers
void DesignConfig::setFrameReader(QSharedPointer<FrameReader> frameReader)
{
	frameReader_ = frameReader;
}

QSharedPointer<FrameReader> DesignConfig::getFrameReader()
{
	return frameReader_;
}

void DesignConfig::setLfpReader(QSharedPointer<LfpReader> lfpReader)
{
	lfpReader_ = lfpReader;
}

void DesignConfig::setRewardReader(QSharedPointer<RewardReader> rewardReader)
{
	rewardReader_ = rewardReader;
}

void DesignConfig::setSpikeReader(QSharedPointer<SpikeReader> spikeReader)
{
	spikeReader_ = spikeReader;
}

QSharedPointer<LfpReader> DesignConfig::getLfpReader()
{
	return lfpReader_;
}

QSharedPointer<RewardReader> DesignConfig::getRewardReader()
{
	return rewardReader_;
}

QSharedPointer<SpikeReader> DesignConfig::getSpikeReader()
{
	return spikeReader_;
}

void DesignConfig::setSignalReader(QString name, QSharedPointer<SignalReader> signalReader)
{
	signalReaders_[name.toLower()] = signalReader;
}

void DesignConfig::clearSignalReaders()
{
	signalReaders_.clear();
}

QSharedPointer<SignalReader> DesignConfig::getSignalReader(QString name)
{
	if(!signalReaders_.contains(name.toLower()))
		return QSharedPointer<SignalReader>();
	return signalReaders_[name.toLower()];
}

QList<AssetInfo> DesignConfig::getElementInfo()
{
	if(!elemInfo_.size())
		fromXml(toXml());
	return elemInfo_;
}
QList<PropInfo> DesignConfig::getPropertyInfo()
{
	if(!propInfo_.size())
		fromXml(toXml());
	return propInfo_;
}
QList<TransInfo> DesignConfig::getTransitionInfo()
{
	if(!transInfo_.size())
		fromXml(toXml());
	return transInfo_;
}