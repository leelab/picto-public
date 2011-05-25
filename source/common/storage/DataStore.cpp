#include <QVariant>

#include "DataStore.h"
#include "../property/Property.h"
#include "PropertyFactory.h"

namespace Picto {

DataStore::DataStore():
Asset()
{
	propertyContainer_ = PropertyContainer::create("DataStore");
}

//Autoserialization Stuff-------------------------------------------------------------

//!!!!!CURRENTLY THE ONLY TIME THE EXPERIMENT IS GETTING SERIALIZED IS WHEN IT IS
//ALTERED IN THE TEXT WINDOW AND THEN A NEW EXPERIMENT IS ACTIVATED WITH THE BUTTON.
//THIS CURRENTLY NEVER HAPPENS AND ALSO MAY NOT HAVE BEEN WORKING IN THE FIRST PLACE.
//YOU HAVE SET UP SCRIPTELEMENT TO USE THIS SERIALIZEASXML CODE AS A TEST SO YOU'LL
//WANT TO MAKE THAT STUFF WORK OR AT LEAST DO SOMETHING ELSE SO YOU CAN TEST THIS.
bool DataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(isDeleted())
		return true;
	if(isNew())
		tagText_ = QString("<%1/>").arg(defaultTagName());
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(tagText_));

	//Move read pointer to the first token
	while(!xmlReader->isStartElement())
		xmlReader->readNext();

	//Write out first token (ie. My tag name).
	xmlStreamWriter->writeCurrentToken(*xmlReader);

	//Get this DataStore's tag name from the stored xml.
	myTagName_ = xmlReader->name().toString();
	
	bool returnVal = true;	
	//Make a list of iterators to use for iterating through the children_ data.
	//Make an "end iterator" list as well to find out when you're done.
	QMap<QString,QList<QSharedPointer<Asset>>::iterator> readIterators;
	QMap<QString,QList<QSharedPointer<Asset>>::iterator> readEndIterators;
	for(QMap<QString,QList<QSharedPointer<Asset>>>::iterator iter=children_.begin();iter!=children_.end();iter++)
	{
		readIterators[iter.key()] = (*iter).begin();
		readEndIterators[iter.key()] = (*iter).end();
	}
	QMap<QString,QList<QSharedPointer<Asset>>::iterator>::iterator dataIter, endIter;

	//DO ALL THE FANCY READING AND WRITING OF XML TO KEEP ORDERING INTACT HERE
	while(xmlReader->readNext() && !(xmlReader->isEndElement() && (xmlReader->name().toString() == myTagName_)) && !xmlReader->atEnd())
	{
		//Continue loop if we're not at a start element
		if(!xmlReader->isStartElement())
		{
			xmlStreamWriter->writeCurrentToken(*xmlReader);// Serialize out whatever we read
			continue;
		}
		//Check to see if this token corresponds to a child.
		//Get Tag name
		QString name = xmlReader->name().toString();
		//See if there's a read iteratorfor this tag.
		if(!readIterators.contains(name))
		{
			//It must be a comment.  Start the process of writing it out.
			xmlStreamWriter->writeCurrentToken(*xmlReader);
			continue;
		}
		//Get the Asset for the tag 
		Q_ASSERT_X(readIterators[name] != readEndIterators[name],"DataStore::serializeAsXml","The original deserialized xml string contains a tag for which there are no remaining children to deserialize.");
		QSharedPointer<Asset> child = *(readIterators[name]++);
		Q_ASSERT_X(!child.isNull(),"DataStore::serializeAsXml","A Asset child pointer in this DataStore's list is null.");
		//Deserialize all new children until you get to one that isn't new
		while(child->isNew())
		{
			child->toXml(xmlStreamWriter);
			Q_ASSERT_X(readIterators[name] != readEndIterators[name],"DataStore::serializeAsXml","The original deserialized xml string contains a tag for which there are no remaining children to deserialize.");
			child = *(readIterators[name]++);		
		}
		//This is the child represented by this tag.  Deserialize it.
		child->toXml(xmlStreamWriter);
		//Move read pointer forward.  The next thing in it should be an end tag, and we don't want
		//to write that out because the child Asset already wrote out its end tag.
		xmlReader->readNext();
	}

	//Write out everything that wasn't yet written that needs to be
	dataIter = readIterators.begin();
	endIter = readEndIterators.begin();
	QSharedPointer<Asset> childAsset;
	while(dataIter!=readIterators.end())
	{
		while((*dataIter) != (*endIter))
		{
			childAsset = (*(*dataIter));
			//If anything was deleted, it must have been in the original XML,
			//otherwise the system would have removed it from our list.
			Q_ASSERT(!childAsset->isDeleted());
			//If the childAsset wasEdited and it wasn't deserialized
			//in from the XML, it must have been a default value that was
			//altered by the user.  It must now be serialized out.  If
			//the childAsset isNew() it must have been added by the user since
			//we serialized in and must also be serialized out.
			if(childAsset->wasEdited() || childAsset->isNew())
				childAsset->toXml(xmlStreamWriter);
			(*dataIter)++;
		}
		dataIter++;
		endIter++;
	}
	//Write out this Datastore's closing tag.
	xmlStreamWriter->writeCurrentToken(*xmlReader);	
	return returnVal;
}
bool DataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Create XMLStreamWriter to store this DataStore's tag text in a string
	tagText_ = "";
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&tagText_));
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the start tag to our tagText.

	//Initilialize this DataStore's deserialization
	bool returnVal = true;
	children_.clear();
	foreach(QSharedPointer<AssetFactory> factory,factories_)
	{
		if(factory.isNull())
			continue;	//It is a placeholder tag
		factory->startSourcing();
	}

	//Get Start tag name (depending on who the parent is, they may have given us different names.
	myTagName_ = xmlStreamReader->name().toString();
	//Make sure its a start tag
 	if(!xmlStreamReader->isStartElement())
	{
		addError(myTagName_.toAscii(),QString("Incorrect tag, expected <%1>").arg(myTagName_).toAscii(),xmlStreamReader);
		returnVal = false;
		return returnVal;
	}

	//Make sure that there are no attributes besides "type"
	int numAttributes = xmlStreamReader->attributes().size();
	if((numAttributes > 1) || ((numAttributes == 1) && !xmlStreamReader->attributes().hasAttribute("type")))
	{
		addError(myTagName_.toAscii(),QString("Incorrect attribute(s), only \"type\" attributes are allowed"),xmlStreamReader);
		returnVal = false;
		return returnVal;
	}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	bool childWithSameTag = false;	//If this is true, we just deserialized a child with the same tag, so we should ignore its end tag.
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == myTagName_) && !childWithSameTag) && !xmlStreamReader->atEnd())
	{
		childWithSameTag = false;
		//Move forward in XMLStream
		if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
		{
			addError(myTagName_.toAscii(),QString("XML syntax has been violated.").toAscii(),xmlStreamReader);
			return false;
		}
		xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets add the current tag to our tagText.
		//Continue loop if we're not at a start element
		if(!xmlStreamReader->isStartElement())
			continue;
		//Get Tag name
		QString name = xmlStreamReader->name().toString();
		//See if there's a Asset factory for this tag.
		if(!factories_.contains(name))
		{
			//There's no entry in our factory list for this tag.  Syntax Error.
			//Report error and attempt to move XMLStream pointer to the end of this tag
			returnVal = false;
			addError(myTagName_.toAscii(),QString("The tag: <%1> is invalid in the %2 object definition").arg(name).arg(myTagName_).toAscii(),xmlStreamReader);
			while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == name)) && !xmlStreamReader->atEnd())
			{
				xmlStreamReader->readNext();
			}
			xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets add the closing tag from the child to our tagText.
			continue;
		}
		//Check if its a child with the same tag as us.  If so, we'll need to ignore the end tag when we read it on the next loop.
		if(name == myTagName_)
			childWithSameTag = true;
		
		//Get the factory for the tag 
		QSharedPointer<AssetFactory> factory = factories_.value(name);
		if(factory.isNull())
		{
			//If factory is null, this is a place holder tag to be ignored.  
			Q_ASSERT_X(!childWithSameTag,"DataStore::deserializeFromXml",QString("The tag: <%1> has the same name as its parent and cannot be used as a placeholder").arg(name).toAscii());
			continue;
		}
		//Get the new Asset
		//
		//The command below will put an empty string into type if the "type" attribute
		//isn't found.  This should map to the default value in the factory.
		QString type = xmlStreamReader->attributes().value("type").toString();
		QString error = "";
		QSharedPointer<Asset> newChild = factory->getAsset(error,type);
		//If the new child is Null then that factory must not support the input type.
		//Report an error and attempt to continue deserialization to catch any other errors.
		if(newChild.isNull())
		{
			returnVal = false;
			addError(myTagName_.toAscii(),QString(error),xmlStreamReader);
			//Attempt to loop until the end tag of the bad tag.
			while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == name)) && !xmlStreamReader->atEnd())
			{
				xmlStreamReader->readNext();
			}
			xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets add the closing tag from the child to our tagText.
			continue;
		}
		//We created the new child.  Lets deserialize it.
		if(!newChild->fromXml(xmlStreamReader))
		{
			//Child deserialization failed.  For now, just exit.
			//We may want to attempt to continue deserialization in the futures in order
			//to present other errors.
			returnVal = false;
			return false;
		}
		xmlWriter->writeCurrentToken(*xmlStreamReader);	// Lets add the closing tag from the child to our tagText.
		//Add the new child Asset to the children map, and continue 
		//the loop
		children_[name].push_back(newChild);
		connect(newChild.data(),SIGNAL(edited()),this,SLOT(childEdited()));
	}
	//Make sure we didn't finish document
	if(xmlStreamReader->atEnd())
	{
		returnVal = false;
		addError(myTagName_.toAscii(), "Unexpected end of document", xmlStreamReader);
	}
	//Generate any default Assets that were not in the XML
	for(QMap<QString,QSharedPointer<AssetFactory>>::iterator iter = factories_.begin();iter!=factories_.end();iter++)
	{
		if(iter.value().isNull())
			continue;	//Its a placeholder tag
		QSharedPointer<Asset> newChild;
		forever{
			newChild = iter.value()->getRequiredAsset();
			if(newChild.isNull())
				break;
			//Generate new XMLStreamReader that is an empty tag to deserialize the child into a default state.
			QString childTag = QString("<%1/>").arg(iter.key());
			QSharedPointer<QXmlStreamReader> emptyTagXML(new QXmlStreamReader(childTag));
			// Move read pointer to tag.
			while(!emptyTagXML->isStartElement()) 
				emptyTagXML->readNext();
			newChild->fromXml(emptyTagXML);
			//Add the new child Asset to the children map, and continue 
			//the loop
			children_[iter.key()].push_back(newChild);
		}
	}

	if(returnVal)
	{	
		//The XML syntax was fine and all children have validated their code as well.  
		//Lets make sure that the object doesn't contain any other errors (ie. Script Errors, etc)
		returnVal &= validateObject(xmlStreamReader);
		if(!returnVal)
			addError(myTagName_.toAscii(),QString("The %1 Object's Structural XML syntax was correct but there were errors in tag contents.").arg(myTagName_).toAscii(),xmlStreamReader);
	}
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the end tag to our tagText.
	return returnVal;
}

void DataStore::setDeleted()
{
	emit deleted();
	foreach(QList<QSharedPointer<Asset>> AssetList,children_)
	{
		foreach(QSharedPointer<Asset> child,AssetList)
		{
			child->setDeleted();
		}
	}
}

void DataStore::initializePropertiesToDefaults()
{
	//Generate new XMLStreamReader that is an empty tag to deserialize the child into a default state.
	QString emptyTag = QString("<%1/>").arg(defaultTagName());
	QSharedPointer<QXmlStreamReader> emptyTagXML(new QXmlStreamReader(emptyTag));
	// Move read pointer to tag.
	while(!emptyTagXML->isStartElement()) 
		emptyTagXML->readNext();
	bool success = fromXml(emptyTagXML);
	Q_ASSERT_X(success,"DataStore::initializePropertiesToDefaults","Failed to initialize DataStore object to default values.");
}

void DataStore::AddDefinableProperty(
		QString tagName, 
		QVariant defaultValue, 
		QMap<QString,QVariant> attributeMap,
		int minNumOfThisType, 
		int maxNumOfThisType
		)
{
	AddDefinableProperty(defaultValue.type(),tagName,defaultValue,attributeMap,minNumOfThisType,maxNumOfThisType);
}

void DataStore::AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QString singleAttributeName,
		QVariant singleAttributeValue,
		int minNumOfThisType, 
		int maxNumOfThisType
		)
{
	QMap<QString,QVariant> attributeMap;
	attributeMap[singleAttributeName] = singleAttributeValue;
	AddDefinableProperty(type,tagName,defaultValue,attributeMap,minNumOfThisType,maxNumOfThisType);
}

void DataStore::AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue,
		int minNumOfThisType, 
		int maxNumOfThisType
		)
{
	AddDefinableProperty(type,tagName,defaultValue,QMap<QString,QVariant>(),minNumOfThisType,maxNumOfThisType);
}

void DataStore::AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QMap<QString,QVariant> attributeMap,
		int minNumOfThisType, 
		int maxNumOfThisType
		)
{
	QSharedPointer<AssetFactory> propFactory(new PropertyFactory(
														minNumOfThisType,
														maxNumOfThisType,
														propertyContainer_,
														type,
														tagName,
														defaultValue,
														attributeMap)
													);

	AddDefinableObjectFactory(tagName,propFactory);
}

void DataStore::AddDefinableObject(QString tagName, QSharedPointer<Asset> object)
{
	QSharedPointer<AssetFactory> factory(new AssetFactory(object));
	AddDefinableObjectFactory(tagName,factory);
}

void DataStore::AddDefinableObjectFactory(QString tagName, QSharedPointer<AssetFactory> factory)
{
	factories_[tagName]=factory;
}

void DataStore::DefinePlaceholderTag(QString tagName)
{
	factories_[tagName] = QSharedPointer<AssetFactory>();
}

QList<QSharedPointer<Asset>> DataStore::getGeneratedChildren(QString tagName)
{
	if(!children_.contains(tagName))
		return QList<QSharedPointer<Asset>>();
	return children_[tagName];
}

bool DataStore::hasChildrenOfType(QString tagName)
{
	return children_.contains(tagName);
}

void DataStore::childEdited()
{
	emit edited();
}
//------------------------------------------------------------------------------------

//void DataStore::serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
//						QString name, QPoint point)
//{
//	Q_ASSERT(!name.isEmpty());
//	xmlStreamWriter->writeStartElement(name);
//	xmlStreamWriter->writeAttribute("datatype", "QPoint");
//	xmlStreamWriter->writeAttribute("x",QString("%1").arg(point.x()));
//	xmlStreamWriter->writeAttribute("y",QString("%1").arg(point.y()));
//	xmlStreamWriter->writeEndElement();
//
//}
//
//void DataStore::serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
//						QString name, QRect rect)
//{
//	Q_ASSERT(!name.isEmpty());
//	xmlStreamWriter->writeStartElement(name);
//	xmlStreamWriter->writeAttribute("datatype", "QRect");
//	xmlStreamWriter->writeAttribute("x",QString("%1").arg(rect.x()));
//	xmlStreamWriter->writeAttribute("y",QString("%1").arg(rect.y()));
//	xmlStreamWriter->writeAttribute("width",QString("%1").arg(rect.width()));
//	xmlStreamWriter->writeAttribute("height",QString("%1").arg(rect.height()));
//	xmlStreamWriter->writeEndElement();
//
//}
//
//void DataStore::serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
//					QString name, QColor color)
//{
//	Q_ASSERT(!name.isEmpty());
//	xmlStreamWriter->writeStartElement(name);
//	xmlStreamWriter->writeAttribute("datatype", "QColor");
//	xmlStreamWriter->writeAttribute("R",QString("%1").arg(color.red()));
//	xmlStreamWriter->writeAttribute("G",QString("%1").arg(color.green()));
//	xmlStreamWriter->writeAttribute("B",QString("%1").arg(color.blue()));
//	xmlStreamWriter->writeAttribute("A",QString("%1").arg(color.alpha()));
//	xmlStreamWriter->writeEndElement();
//
//}
//
//
//QPoint DataStore::deserializeQPoint(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	QPoint point;
//	point.setX(xmlStreamReader->attributes().value("x").toString().toInt());
//	point.setY(xmlStreamReader->attributes().value("y").toString().toInt());
//	return point;
//}
//
//QRect DataStore::deserializeQRect(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	QRect rect;
//	rect.setX(xmlStreamReader->attributes().value("x").toString().toInt());
//	rect.setY(xmlStreamReader->attributes().value("y").toString().toInt());
//	rect.setWidth(xmlStreamReader->attributes().value("width").toString().toInt());
//	rect.setHeight(xmlStreamReader->attributes().value("height").toString().toInt());
//	return rect;
//}
//
//QColor DataStore::deserializeQColor(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	QColor color;
//	color.setRed(xmlStreamReader->attributes().value("R").toString().toInt());
//	color.setGreen(xmlStreamReader->attributes().value("G").toString().toInt());
//	color.setBlue(xmlStreamReader->attributes().value("B").toString().toInt());
//	color.setAlpha(xmlStreamReader->attributes().value("A").toString().toInt());
//	return color;
//}

}; //namespace Picto
