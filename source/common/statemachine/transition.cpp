#include "Transition.h"
#include "../memleakdetect.h"

namespace Picto
{

Transition::Transition()
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult","");
	AddDefinableProperty("Destination","");
}

Transition::Transition(QString source, QString sourceResult, QString destination)
{
	AddDefinableProperty("Source",source);
	AddDefinableProperty("SourceResult",sourceResult);
	AddDefinableProperty("Destination",destination);
	initializePropertiesToDefaults();
	setEdited();
	//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
	propertyContainer_->getProperty("Source")->setEdited();
	propertyContainer_->getProperty("SourceResult")->setEdited();
	propertyContainer_->getProperty("Destination")->setEdited();
}

QSharedPointer<Asset> Transition::Create()
{
	return QSharedPointer<Asset>(new Transition());
}


void Transition::setSource(QString source) 
{ 
	propertyContainer_->setPropertyValue("Source",source);
	disconnect(sourceAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	sourceAsset_ = QSharedPointer<Asset>();
}
void Transition::setSourceResult(QString sourceResult) 
{ 
	propertyContainer_->setPropertyValue("SourceResult",sourceResult);
	disconnect(sourceResultAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	sourceResultAsset_ = QSharedPointer<Asset>();
}
void Transition::setDestination(QString destination) 
{
	propertyContainer_->setPropertyValue("Destination",destination);
	disconnect(destinationAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	destinationAsset_ = QSharedPointer<Asset>();
}

void Transition::setSource(QSharedPointer<Asset> source)
{
	if(!sourceAsset_.isNull())
		sourceAsset_->disconnect(this);
	sourceAsset_ = source;
	if(!sourceAsset_.isNull())
	{
		connect(sourceAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
		connect(sourceAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
	}
	QString name = sourceAsset_?sourceAsset_->getName():"";
	if(propertyContainer_->getPropertyValue("Source").toString() != name)
		propertyContainer_->setPropertyValue("Source",sourceAsset_?name:"");
}
void Transition::setSourceResult(QSharedPointer<Asset> sourceResult)
{
	if(!sourceResultAsset_.isNull())
		sourceResultAsset_->disconnect(this);
	sourceResultAsset_ = sourceResult;
	if(!sourceResultAsset_.isNull())
	{
		connect(sourceResultAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
		connect(sourceResultAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
	}
	QString name = sourceResultAsset_?sourceResultAsset_->getName():"";
	if(propertyContainer_->getPropertyValue("SourceResult").toString() != name)
		propertyContainer_->setPropertyValue("SourceResult",sourceResultAsset_?name:"");

}
void Transition::setDestination(QSharedPointer<Asset> destination)
{
	Q_ASSERT(destination);
	if(!destinationAsset_.isNull())
		destinationAsset_->disconnect(this);
	destinationAsset_ = destination;
	Q_ASSERT(destination);
	connect(destinationAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	connect(destinationAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
	QString name = destinationAsset_->getName();
	if(propertyContainer_->getPropertyValue("Destination").toString() != name)
		propertyContainer_->setPropertyValue("Destination",name);
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

}

void Transition::linkedAssetDeleted()
{
	setDeleted();
}

}