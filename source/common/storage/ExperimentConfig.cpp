#include "ExperimentConfig.h"
#include "Asset.h"
#include "../statemachine/Transition.h"

using namespace Picto;

ExperimentConfig::ExperimentConfig()
{
	lastUsedId_ = 0;
	allowIdDuplication_ = true;
}

bool ExperimentConfig::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	QSharedPointer<Asset> sAsset;
	QSharedPointer<Transition> sTAsset;
	
	xmlStreamWriter->writeStartElement("ExperimentConfig");

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

bool ExperimentConfig::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "ExperimentConfig")
	{
		addError("ExperimentConfig","Incorrect tag, expected <ExperimentConfig>",xmlStreamReader);
		return false;
	}
	xmlStreamReader->readNext();	//Move past the ExperimentConfig starting tag
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "ExperimentConfig") && !xmlStreamReader->atEnd())
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
			xmlStreamReader->readNext();	//Move past the ExperimentConfig starting tag
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
					addError("ExperimentConfig","Property missing i (index) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("n"))
				{
					propInf.name = xmlStreamReader->attributes().value("n").toString();
				}
				else
				{
					addError("ExperimentConfig","Property missing n (name) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					propInf.parent = xmlStreamReader->attributes().value("p").toString().toInt();
				}
				else
				{
					addError("ExperimentConfig","Property missing p (parent) attribute",xmlStreamReader);
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
					addError("ExperimentConfig","Transition missing i (index) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("n"))
				{
					transInf.name = xmlStreamReader->attributes().value("n").toString();
				}
				else
				{
					addError("ExperimentConfig","Transition missing n (name) attribute",xmlStreamReader);
					return false;
				}
				
				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					transInf.parent = xmlStreamReader->attributes().value("p").toString().toInt();
				}
				else
				{
					addError("ExperimentConfig","Transition missing p (parent) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("s"))
				{
					transInf.source = xmlStreamReader->attributes().value("s").toString();
				}
				else
				{
					addError("ExperimentConfig","Transition missing s (source) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("r"))
				{
					transInf.result = xmlStreamReader->attributes().value("r").toString();
				}
				else
				{
					addError("ExperimentConfig","Transition missing r (source result) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("d"))
				{
					transInf.dest = xmlStreamReader->attributes().value("d").toString();
				}
				else
				{
					addError("ExperimentConfig","Transition missing d (destination) attribute",xmlStreamReader);
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
					addError("ExperimentConfig","Element missing i (index) attribute",xmlStreamReader);
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					elemInf.path = xmlStreamReader->attributes().value("p").toString();
				}
				else
				{
					addError("ExperimentConfig","Element missing p (path) attribute",xmlStreamReader);
					return false;
				}
				elemInfo_.append(elemInf);
				xmlStreamReader->readNext();
			}
		}
		else
		{
			addError("ExperimentConfig","Invalid Tag: " + name.toAscii(),xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void ExperimentConfig::disallowIdDuplication()
{
	allowIdDuplication_ = false;
}

int ExperimentConfig::getNewDataId()
{
	++lastUsedId_;
	//Increment the id until you find an unused one
	while(assetsById_.contains(lastUsedId_))
	{
		lastUsedId_++;
	}
	return lastUsedId_;
}

void ExperimentConfig::addAsset(QSharedPointer<Asset> asset, bool serialized)
{
	QWeakPointer<Asset> wAsset(asset);
	Q_ASSERT(!assetHash_.contains(asset.data()));
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
	if(serialized)
		unsortedIdSerializedAssets_.append(wAsset);
	else
		unsortedIdUnSerializedAssets_.append(wAsset);
}

void ExperimentConfig::addManagedSerializedAsset(QSharedPointer<Asset> asset)
{
	addAsset(asset,true);
}

void ExperimentConfig::addManagedUnserializedAsset(QSharedPointer<Asset> asset)
{
	addAsset(asset,false);
}

void ExperimentConfig::fixDuplicatedAssetIds()
{
	if(allowIdDuplication_)
		return;
	QSharedPointer<Asset> sAsset;
	int id;
	QList<QWeakPointer<Asset>>* currList;
	for(int i=0;i<2;i++)
	{
		switch(i)
		{
			case 0: currList = &unsortedIdSerializedAssets_; break;
			case 1: currList = &unsortedIdUnSerializedAssets_; break;
		}
		while(currList->size())
		{
			QWeakPointer<Asset> wAsset = currList->takeFirst();
			sAsset = QSharedPointer<Asset>(wAsset);
			if(!sAsset)
				continue;	//This asset was already deleted
			id = sAsset->getAssetId();
			if((id == 0 ) || assetsById_.contains(id))	//This assets id is the zero (reserved) or has already been used.
			{
				id = getNewDataId();		//Get a new asset id and set it to the asset.
				sAsset->setAssetId(id);
			}
			assetsById_[id] = wAsset;		//Add the asset to the id map with its id.
		}
	}
}

QSharedPointer<Asset> ExperimentConfig::getAsset(int id)
{
	QSharedPointer<Asset> returnVal;
	if(assetsById_.contains(id))
		returnVal = assetsById_[id];
	return returnVal;
}



QList<AssetInfo> ExperimentConfig::getElementInfo()
{
	if(!elemInfo_.size())
		fromXml(toXml());
	return elemInfo_;
}
QList<PropInfo> ExperimentConfig::getPropertyInfo()
{
	if(!propInfo_.size())
		fromXml(toXml());
	return propInfo_;
}
QList<TransInfo> ExperimentConfig::getTransitionInfo()
{
	if(!transInfo_.size())
		fromXml(toXml());
	return transInfo_;
}