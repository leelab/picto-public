#include "Transition.h"

namespace Picto
{
int Transition::lastTransitionId_ = -1;

Transition::Transition()
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult","");
	AddDefinableProperty("Destination","");
	id_ = ++lastTransitionId_;
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
	connect(sourceAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	connect(sourceAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
}
void Transition::setSourceResult(QSharedPointer<Asset> sourceResult)
{
	if(!sourceResultAsset_.isNull())
		sourceResultAsset_->disconnect(this);
	sourceResultAsset_ = sourceResult;
	connect(sourceResultAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	connect(sourceResultAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
}
void Transition::setDestination(QSharedPointer<Asset> destination)
{
	if(!destinationAsset_.isNull())
		destinationAsset_->disconnect(this);
	destinationAsset_ = destination;
	connect(destinationAsset_.data(),SIGNAL(edited()),this,SLOT(setValuesFromAssets()));
	connect(destinationAsset_.data(),SIGNAL(deleted()),this,SLOT(linkedAssetDeleted()));
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

/*!	\brief Turns a transition into XML
 *
 *	The XML fragment will look like this
 *	<Transition>
 *		<Source>SourceElementName</Source>
 *		<SourceResult>SourceResultName</SourceResult>
 *		<Destination>SourceElementName</Destination>
 *	</Transition>
 */
//bool Transition::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Transition");
//	xmlStreamWriter->writeTextElement("Source",source_);
//	xmlStreamWriter->writeTextElement("SourceResult",sourceResult_);
//	xmlStreamWriter->writeTextElement("Destination",destination_);
//	xmlStreamWriter->writeEndElement(); //Transition
//	return true;
//}
//
//bool Transition::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->getName() != "Transition")
//	{
//		addError("Transition","Incorrect tag, expected <Transition>",xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->getName().toString() == "Transition") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->getName().toString();
//		if(name == "Source")
//		{
//			source_ = xmlStreamReader->readElementText();
//		}
//		else if(name == "SourceResult")
//		{
//			sourceResult_ = xmlStreamReader->readElementText();
//		}
//		else if(name == "Destination")
//		{
//			destination_ = xmlStreamReader->readElementText();
//		}
//		else
//		{
//			addError("Transition", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Transition", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

void Transition::postSerialize()
{
	DataStore::postSerialize();
}

bool Transition::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

void Transition::setValuesFromAssets()
{
	propertyContainer_->setPropertyValue("Source",sourceAsset_->getName());
	propertyContainer_->setPropertyValue("SourceResult",sourceResultAsset_->getName());
	propertyContainer_->setPropertyValue("Destination",destinationAsset_->getName());

}

void Transition::linkedAssetDeleted()
{
	setDeleted();
}

}