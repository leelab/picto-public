#include "VisualElement.h"

#include <QScriptValue>

namespace Picto {

VisualElement::VisualElement() :
	shouldUpdateCompositingSurfaces_(true),
	visible_(true)
{
	AddDefinableProperty(QVariant::Int,"Order",0);
}

VisualElement::~VisualElement()
{
}

QPoint VisualElement::getPosition()
{
	QVariant positionVariant = propertyContainer_->getPropertyValue("Position");

	if(positionVariant == QVariant::Invalid)
	{
		return QPoint(0,0);
	}
	else
	{
		return positionVariant.toPoint();
	}
}

QRect VisualElement::getBoundingRect()
{
	return image_.rect();
}



void VisualElement::setPosition(QPoint position)
{
	propertyContainer_->setPropertyValue("Position",position);
}

QColor VisualElement::getColor()
{
	return propertyContainer_->getPropertyValue("Color").value<QColor>();
}

void VisualElement::setColor(QColor color)
{
	propertyContainer_->setPropertyValue("Color",color);
}

void VisualElement::addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface)
{
	compositingSurface->convertImage(image_);
	compositingSurfaces_[surfaceType] = compositingSurface;
}

QSharedPointer<CompositingSurface> VisualElement::getCompositingSurface(QString surfaceType)
{
	if(compositingSurfaces_.contains(surfaceType))
	{
		return compositingSurfaces_.value(surfaceType);
	}
	else
	{
		return QSharedPointer<CompositingSurface>();
	}
}

void VisualElement::updateCompositingSurfaces()
{
	foreach(QSharedPointer<CompositingSurface> compositingSurface, compositingSurfaces_.values())
	{
		compositingSurface->convertImage(image_);
	}
}

//If updateAnimation isn't implemented in the derived class, we can simply return.
void VisualElement::updateAnimation(int frame, QTime elapsedTime)
{
	Q_UNUSED(frame);
	Q_UNUSED(elapsedTime);
	if(shouldUpdateCompositingSurfaces_)
	{
		updateCompositingSurfaces();
	}
	return;
}

/*!	\brief Resets the VisualElement
 *
 *	Since VisualElements can be modified by scripts, resetting them returns them
 *	to their initial state.  This basically just reloads the PropertyContainer
 *	with the initial properties
 */
void VisualElement::reset()
{
	restoreProperties();
}

/*	\brief Stores the elements properties in a map.
 *
 *	Since everything important about a VisualElement is stored in its porperties,
 *	we can "back up" the element by recording those properties.  Since a property 
 *	can be set with nothing more than a name and value, we store all of that info 
 *	in a QMap<QString, QVariant>.
 */
void VisualElement::backupProperties()
{
	initialProperties_.clear();

	//iterate through the properties storing their values by name
	QStringList properties = propertyContainer_->getPropertyList();

	foreach(QString name, properties)
	{
		initialProperties_[name] = propertyContainer_->getPropertyValue(name);
	}
}

/* \brief Relads the property container with the values backed up in backupProperties()
 *
 *	This basically iterates through the backed up properties and resets the values
 *	in the property container.
 */
void VisualElement::restoreProperties()
{
	QStringList properties = propertyContainer_->getPropertyList();

	QMap<QString, QVariant>::iterator propItr = initialProperties_.begin();
	while(propItr != initialProperties_.end())
	{
		//There should never be a property in initialProperties_ that doesn't
		//exist in the property container
		Q_ASSERT(properties.contains(propItr.key()));
		propertyContainer_->setPropertyValue(propItr.key(),propItr.value());
		propItr++;
	}
}

/*! \brief Turns the VisualElement object into an XML fragment.
 *
 *	Since VisualElements are defined soley by their properties, we can handle
 *	serialization in this base class.  The only issue with this approach is that 
 *	there are an infinite number of property data types, and we will only
 *	include code for turning some of them into XML.  So, if you create a new
 *	VisualElement that has some sort of non-standard data type (for example, 
 *	an enum, or special struct), then this code will need to be modified.  To
 *	prevent accidental bugs, this function will experience an assertion failure
 *	if it encounters an unknown data type in a the properties.
 *
 *	It should also be noted that the function (as of its initial development)
 *	only supports porperties, so if a Visual Element uses Property Attributes
 *	we will have problems.
 *
 *	The generated XML for a Visual element will Look like this
 *	<VisualElement type="Box Graphic">
 *		<Name datatype="QString">LeftBox</Name>
 *		<Position datatype="QPoint" x="400" y="400">
 *		<Dimensions datatype="QRect" x="0" y="0" width="100" height="100"/>
 *		<Color datatype="QColor" R="255" G="0" B="0" A="255"/>
 *	</VisualElement>

 */
//bool VisualElement::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("VisualElement");
//
//	//add the visual element's type (BoxGraphic, ARrowgraphic, etc)
//	xmlStreamWriter->writeAttribute("type",propertyContainer_->getContainerName());
//
//	//Get the list of properties
//	QList<QString> propList = propertyContainer_->getPropertyList();
//
//	//loop through the property list
//	QString propName;
//	foreach(propName, propList)
//	{
//		QVariant propVal = propertyContainer_->getPropertyValue(propName);
//
//		//figure out what data type this is, and output it appropriately
//		switch(propVal.type())
//		{
//		case QVariant::Point:
//			serializeQPoint(xmlStreamWriter, propName, propVal.toPoint());
//			break;
//		case QVariant::Rect:
//			serializeQRect(xmlStreamWriter, propName, propVal.toRect());
//			break;
//		case QVariant::Color:
//			serializeQColor(xmlStreamWriter, propName, propVal.value<QColor>());
//			break;
//		case QVariant::String:
//		case QVariant::Int:
//			//xmlStreamWriter->writeTextElement(propName,propVal.toString());
//			xmlStreamWriter->writeStartElement(propVal.typeName());
//			xmlStreamWriter->writeAttribute("name",propName);
//			xmlStreamWriter->writeCharacters(propVal.toString());
//			xmlStreamWriter->writeEndElement();
//			break;
//		default:
//			//If you have reacehd this point, it means that we have encountered
//			//some sort of unexpected data type in one of the VisualElements. 
//			//The developer will need to add support for the new data type.
//			return false;
//		}
//	}
//
//	//bool success = serializePropertiesAsXML(xmlStreamWriter);
//
//	xmlStreamWriter->writeEndElement(); //VisualElement
//
//	return true;
//}
//
//
///*! \brief Converts an XML fragment into a VisualElement
// *
// *	This code handles the conversion of XML into real objects. This function
// *	will be called after the parent deserializer has read a <VisualElement type="blah">
// *	tag.  The parent deserializer will then instanatiate a VisualElement of the 
// *	appropriate type and call us.  We will return with the xmlStreamREader's current token
// *	being the </VisualElement>tag.
// *
// */
//bool VisualElement::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "VisualElement")
//	{
//		addError("VisualElement","Incorrect tag, expected <VisualElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != propertyContainer_->getContainerName())
//	{
//		addError("VisualElement","Incorrect type of VisualElement, expected "+propertyContainer_->getContainerName(),xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "VisualElement") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//do nothing unless we're looking at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		QString type = xmlStreamReader->attributes().value("datatype").toString();
//
//		//deserialze the property based on the type of the current property value
//		if(type == "QPoint")
//		{
//			QPoint point;
//			point = deserializeQPoint(xmlStreamReader);
//			propertyContainer_->setPropertyValue(name,point);
//		}
//		else if(type == "QRect")
//		{
//			QRect rect;
//			rect = deserializeQRect(xmlStreamReader);
//			propertyContainer_->setPropertyValue(name,rect);
//		}			
//		else if(type == "QColor")
//		{
//			QColor color;
//			color = deserializeQColor(xmlStreamReader);
//			propertyContainer_->setPropertyValue(name,color);
//		}
//		else if(type == "QString")
//		{
//			QString string;
//			string = xmlStreamReader->readElementText();
//			propertyContainer_->setPropertyValue(name,string);
//		}
//		else if(type == "int")
//		{
//			int intValue;
//			intValue = xmlStreamReader->readElementText().toInt();
//			propertyContainer_->setPropertyValue(name,intValue);
//		}
//		else
//		{
//			//If you have reached this point, it means that we have encountered
//			//some sort of unexpected data type in one of the VisualElements. 
//			//The developer will need to add support for the new data type.
//			addError("VisualElement", "Unexpected tag (likely due to an unsupported data type", xmlStreamReader);
//			return false;
//		}
//
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("VisualElement", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	backupProperties();
//
//	return true;
//}

void VisualElement::postSerialize()
{
	Scriptable::postSerialize();
	backupProperties();
}

bool VisualElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	return true;
}



}; //namespace Picto

