#include "Transition.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Constructs a new Transition object.
 *	\details Adds Source, SourceResult, and Destination Properties to hold the name of the element and result from which the Transition is source and
 *	the element to which the Transition goes.  Also adds SId, SRId and DId Properties to hold the AssetIds of those elements.
 */
Transition::Transition()
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult","");
	AddDefinableProperty("Destination","");
	AddDefinableProperty(QVariant::Int,"SId",0);
	AddDefinableProperty(QVariant::Int,"SRId",0);
	AddDefinableProperty(QVariant::Int,"DId",0);
}

/*! \brief Constructs a new Transition that will transition from the input sourceResult of the input source to the input destination.
 *	\details The Properties described in Transition() are created to store the information.
*/
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

/*! \brief Constructs a new Transition whose Source and Destination will be empty, but whose SourceResult will be equal to the input result value.
 *	\details I believe that this is only used in the Task class to signify when the operator stopped an experiment using EngineAbort.
 *	See Transition() for more information on which Properties are created.
*/
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

/*! \brief Constructs a new Transition whose Destination will be empty, but whose Source and SourceResult will be equal to the input values.
 *	\details I believe that this is only used in the Task class to represent the final Transition in a Task that ends on its own when one of the top level
 *	StateMachine's Result objects is triggered to end the Run.
 *	See Transition() for more information on which Properties are created.
*/
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

/*! \brief Creates a new Transition object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> Transition::Create()
{
	return QSharedPointer<Asset>(new Transition());
}

/*! \brief Sets the source of this Transition to the input Asset.*/
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

/*! \brief Sets the source result of this Transition to the input Asset.*/
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

/*! \brief Sets the destination of this Transition to the input Asset.*/
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

/*! \brief Returns the name of the source of this Transition.*/
QString Transition::getSource() 
{
	return propertyContainer_->getPropertyValue("Source").toString();
}

/*! \brief Returns the name of the source result of this Transition.*/
QString Transition::getSourceResult() 
{ 
	return propertyContainer_->getPropertyValue("SourceResult").toString();
}

/*! \brief Returns the name of the destination of this Transition.*/
QString Transition::getDestination() 
{ 
	return propertyContainer_->getPropertyValue("Destination").toString();;
}

/*! \brief Returns the AssetId of the source of this Transition.*/
int Transition::getSourceId()
{
	return propertyContainer_->getPropertyValue("SId").toInt();
}

/*! \brief Returns the AssetId of the source result of this Transition.*/
int Transition::getSourceResultId()
{
	return propertyContainer_->getPropertyValue("SRId").toInt();
}

/*! \brief Returns the AssetId of the destination of this Transition.*/
int Transition::getDestinationId()
{
	return propertyContainer_->getPropertyValue("DId").toInt();
}

/*! \brief Returns a pointer to the source of this Transition.*/
QSharedPointer<Asset>  Transition::getSourceAsset()
{
	return sourceAsset_;
}

/*! \brief Returns a pointer to the source result of this Transition.*/
QSharedPointer<Asset>  Transition::getSourceResultAsset()
{
	return sourceResultAsset_;
}

/*! \brief Returns a pointer to the destination of this Transition.*/
QSharedPointer<Asset>  Transition::getDestinationAsset()
{
	return destinationAsset_;
}

void Transition::postDeserialize()
{
	DataStore::postDeserialize();
}

/*! \brief Extends DataStore::validateObject() to verify that all Transitions that are included in the Design have a destination.*/
bool Transition::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	if(getDestination().isEmpty())
	{			
		QString errMsg = QString("Transition: %1 does not have a destination.").arg(getName());
		addError(errMsg);
		return false;
	}
	return true;
}

/*! \brief Updtes all Property values according to the stored source, sourceResult and destination Asset pointers.*/
void Transition::setValuesFromAssets()
{
	propertyContainer_->setPropertyValue("Source",sourceAsset_?sourceAsset_->getName():"");
	propertyContainer_->setPropertyValue("SourceResult",sourceResultAsset_?sourceResultAsset_->getName():"");
	propertyContainer_->setPropertyValue("Destination",destinationAsset_->getName());
	propertyContainer_->setPropertyValue("SId",sourceAsset_?sourceAsset_->getAssetId():0);
	propertyContainer_->setPropertyValue("SRId",sourceResultAsset_?sourceResultAsset_->getAssetId():0);
	propertyContainer_->setPropertyValue("DId",destinationAsset_->getAssetId());

}

/*! \brief Called when one of the Assets to which this Transition is linked is deleted.  Causes this Transition to be deleted.*/
void Transition::linkedAssetDeleted()
{
	setDeleted();
}

}