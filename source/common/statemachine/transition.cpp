#include "Transition.h"
#include "../memleakdetect.h"

namespace Picto
{

Transition::Transition()
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult","");
	AddDefinableProperty("Destination","");
	AddDefinableProperty(QVariant::Int,"SId",0);
	AddDefinableProperty(QVariant::Int,"SRId",0);
	AddDefinableProperty(QVariant::Int,"DId",0);
}

Transition::Transition(QSharedPointer<Asset> source, QSharedPointer<Asset> sourceResult, QSharedPointer<Asset> destination)
{
	AddDefinableProperty("Source",source?source->getName():"");
	AddDefinableProperty("SourceResult",sourceResult?sourceResult->getName():"");
	AddDefinableProperty("Destination",destination->getName());
	AddDefinableProperty("SId",source?source->getAssetId():0);
	AddDefinableProperty("SRId",sourceResult?sourceResult->getAssetId():0);
	AddDefinableProperty("DId",destination->getAssetId());
	initializePropertiesToDefaults();
	setEdited();
	//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
	propertyContainer_->getProperty("Source")->setEdited();
	propertyContainer_->getProperty("SourceResult")->setEdited();
	propertyContainer_->getProperty("Destination")->setEdited();
	propertyContainer_->getProperty("SId")->setEdited();
	propertyContainer_->getProperty("SRId")->setEdited();
	propertyContainer_->getProperty("DId")->setEdited();
}

Transition::Transition(QString result)
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult",result);
	AddDefinableProperty("Destination","");
	AddDefinableProperty("SId",0);
	AddDefinableProperty("SRId",-1);
	AddDefinableProperty("DId",0);
	initializePropertiesToDefaults();
	setEdited();
	//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
	propertyContainer_->getProperty("Source")->setEdited();
	propertyContainer_->getProperty("SourceResult")->setEdited();
	propertyContainer_->getProperty("Destination")->setEdited();
	propertyContainer_->getProperty("SId")->setEdited();
	propertyContainer_->getProperty("SRId")->setEdited();
	propertyContainer_->getProperty("DId")->setEdited();
}

Transition::Transition(QSharedPointer<Asset> source,QString sourceResult)
{
	AddDefinableProperty("Source",source->getName());
	AddDefinableProperty("SourceResult",sourceResult);
	AddDefinableProperty("Destination","");
	AddDefinableProperty("SId",source->getAssetId());
	AddDefinableProperty("SRId",-1);
	AddDefinableProperty("DId",0);
	initializePropertiesToDefaults();
	setEdited();
	//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
	propertyContainer_->getProperty("Source")->setEdited();
	propertyContainer_->getProperty("SourceResult")->setEdited();
	propertyContainer_->getProperty("Destination")->setEdited();
	propertyContainer_->getProperty("SId")->setEdited();
	propertyContainer_->getProperty("SRId")->setEdited();
	propertyContainer_->getProperty("DId")->setEdited();
}

QSharedPointer<Asset> Transition::Create()
{
	return QSharedPointer<Asset>(new Transition());
}

void Transition::setSource(QSharedPointer<Asset> source)
{
	Q_ASSERT(sourceAsset_.isNull());
	//sourceAsset_->disconnect(this);
	sourceAsset_ = source;
	if(!sourceAsset_.isNull())
	{
		connect(sourceAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
		connect(sourceAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
	}
	QString name = sourceAsset_?sourceAsset_->getName():"";
	int id = sourceAsset_?sourceAsset_->getAssetId():0;
	if(propertyContainer_->getPropertyValue("Source").toString() != name)
		propertyContainer_->setPropertyValue("Source",sourceAsset_?name:"");
	if(propertyContainer_->getPropertyValue("SId").toInt() != id)
		propertyContainer_->setPropertyValue("SId",id);
}
void Transition::setSourceResult(QSharedPointer<Asset> sourceResult)
{
	Q_ASSERT(sourceResultAsset_.isNull());
	//sourceResultAsset_->disconnect(this);
	sourceResultAsset_ = sourceResult;
	if(!sourceResultAsset_.isNull())
	{
		connect(sourceResultAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
		connect(sourceResultAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
	}
	QString name = sourceResultAsset_?sourceResultAsset_->getName():"";
	int id = sourceResultAsset_?sourceResultAsset_->getAssetId():0;
	if(propertyContainer_->getPropertyValue("SourceResult").toString() != name)
		propertyContainer_->setPropertyValue("SourceResult",sourceResultAsset_?name:"");
	if(propertyContainer_->getPropertyValue("SRId").toInt() != id)
		propertyContainer_->setPropertyValue("SRId",id);

}
void Transition::setDestination(QSharedPointer<Asset> destination)
{
	Q_ASSERT(destination);
	Q_ASSERT(destinationAsset_.isNull());
	//destinationAsset_->disconnect(this);
	destinationAsset_ = destination;
	Q_ASSERT(destination);
	connect(destinationAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	connect(destinationAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
	QString name = destinationAsset_->getName();
	int id = destinationAsset_?destinationAsset_->getAssetId():0;
	if(propertyContainer_->getPropertyValue("Destination").toString() != name)
		propertyContainer_->setPropertyValue("Destination",name);
	if(propertyContainer_->getPropertyValue("DId").toInt() != id)
		propertyContainer_->setPropertyValue("DId",id);
}

QString Transition::getSource() 
{
	return propertyContainer_->getPropertyValue("Source").toString();
}
QString Transition::getSourceResult() 
{ 
	return propertyContainer_->getPropertyValue("SourceResult").toString();
}
QString Transition::getDestination() 
{ 
	return propertyContainer_->getPropertyValue("Destination").toString();;
}

int Transition::getSourceId()
{
	return propertyContainer_->getPropertyValue("SId").toInt();
}

int Transition::getSourceResultId()
{
	return propertyContainer_->getPropertyValue("SRId").toInt();
}

int Transition::getDestinationId()
{
	return propertyContainer_->getPropertyValue("DId").toInt();
}

QSharedPointer<Asset>  Transition::getSourceAsset()
{
	return sourceAsset_;
}

QSharedPointer<Asset>  Transition::getSourceResultAsset()
{
	return sourceResultAsset_;
}

QSharedPointer<Asset>  Transition::getDestinationAsset()
{
	return destinationAsset_;
}

void Transition::postDeserialize()
{
	DataStore::postDeserialize();
}

bool Transition::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	if(getDestination().isEmpty())
	{			
		QString errMsg = QString("Transition: %1 does not have a destination.").arg(getName());
		addError("Transition", errMsg, xmlStreamReader);
		return false;
	}
	return true;
}

void Transition::setValuesFromAssets()
{
	propertyContainer_->setPropertyValue("Source",sourceAsset_?sourceAsset_->getName():"");
	propertyContainer_->setPropertyValue("SourceResult",sourceResultAsset_?sourceResultAsset_->getName():"");
	propertyContainer_->setPropertyValue("Destination",destinationAsset_->getName());
	propertyContainer_->setPropertyValue("SId",sourceAsset_?sourceAsset_->getAssetId():0);
	propertyContainer_->setPropertyValue("SRId",sourceResultAsset_?sourceResultAsset_->getAssetId():0);
	propertyContainer_->setPropertyValue("DId",destinationAsset_->getAssetId());

}

void Transition::linkedAssetDeleted()
{
	setDeleted();
}

}