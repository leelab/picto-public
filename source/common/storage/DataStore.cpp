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

bool DataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	Q_ASSERT_X(tagText_ != "","DataStore::serializeAsXml","This datastore has no tag text, it was either not serialized in or not initialized to default values.");
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
		//See if there's a read iterator for this tag.
		if(!readIterators.contains(name))
		{
			//It must be a comment.  Start the process of writing it out.
			xmlStreamWriter->writeCurrentToken(*xmlReader);
			continue;
		}
		//Get the Asset for the tag 
		Q_ASSERT_X(readIterators[name] != readEndIterators[name],"DataStore::serializeAsXml","The original deserialized xml string contains a tag for which there are no remaining children to deserialize.");
		QSharedPointer<Asset> child = *(readIterators[name]++);
		Q_ASSERT_X(!child.isNull(),"DataStore::serializeAsXml","A child asset pointer in this DataStore's list is null.");
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
			// If the childAsset is not New, the only explanation is that the asset was serialized in under one tag name,
			// then copied to a list of child assets under another tag (as in the case of a choice controller's targets
			// that are also used as results.  In these types of cases, the asset was already serialized under the correct
			// tag name when we were going through our tagText_, so we skip childAssets that are not new here.
			//
			// If the childAsset IS new but was not edited, it must have been an unedited default 
			// value and should not be serialized out.  Otherwise it must have been a 
			// default value that was altered by the user or a new value altogether.  It must now be serialized out.
			if(childAsset->isNew() && childAsset->wasEdited())
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
bool DataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader, bool validate)
{
	//Create XMLStreamWriter to store this DataStore's tag text in a string
	tagText_ = "";
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&tagText_));
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the start tag to our tagText.

	//Initilialize this DataStore's deserialization
	bool returnVal = true;
	children_.clear();
	propertyContainer_->clear();
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
		//We created the new child.  
		//Lets add it to our list of children
		AddChild(name,newChild);
		//Lets deserialize it.
		if(!newChild->fromXml(xmlStreamReader,validate))
		{
			//Child deserialization failed.  For now, just exit.
			//We may want to attempt to continue deserialization in the futures in order
			//to present other errors.
			returnVal = false;
			return false;
		}
		xmlWriter->writeCurrentToken(*xmlStreamReader);	// Lets add the closing tag from the child to our tagText.
		//continue the loop
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
			QString returnedType = "";
			newChild = iter.value()->getRequiredAsset(returnedType);
			if(newChild.isNull())
				break;
			//We created the new child.  
			//Lets add it to our list of children
			AddChild(iter.key(),newChild);
			//Initialize the child into a default state.
			newChild->initializeToDefault(iter.key(),returnedType);
			//continue the loop
		}
	}
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the end tag to our tagText.
	return returnVal;
}

bool DataStore::validateTree()
{
	bool returnVal = true;
	foreach(QList<QSharedPointer<Asset>> childList,children_)
	{
		foreach(QSharedPointer<Asset> child,childList)
		{
			returnVal &= child->validateTree();
		}
	}
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader());
	returnVal &= validateObject(xmlStreamReader);
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
	initializeToDefault();
}

void DataStore::AddDefinableProperty(
		QString tagName, 
		QVariant defaultValue
		)
{
	AddDefinableProperty(defaultValue.type(),tagName,defaultValue,QMap<QString,QVariant>(),1,1);
}

void DataStore::AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QString singleAttributeName,
		QVariant singleAttributeValue
		)
{
	QMap<QString,QVariant> attributeMap;
	attributeMap[singleAttributeName] = singleAttributeValue;
	AddDefinableProperty(type,tagName,defaultValue,attributeMap,1,1);
}

//void DataStore::AddDefinableProperty(
//		int type,
//		QString tagName, 
//		QVariant defaultValue,
//		int minNumOfThisType, 
//		int maxNumOfThisType
//		)
//{
//	AddDefinableProperty(type,tagName,defaultValue,QMap<QString,QVariant>(),minNumOfThisType,maxNumOfThisType);
//}

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
	orderedPropList_.append(tagName);
}

//void DataStore::AddDefinableObject(QString tagName, QSharedPointer<Asset> object)
//{
//	QSharedPointer<AssetFactory> factory(new AssetFactory(object));
//	AddDefinableObjectFactory(tagName,factory);
//}

void DataStore::AddDefinableObjectFactory(QString tagName, QSharedPointer<AssetFactory> factory)
{
	factories_[tagName]=factory;
}

void DataStore::AddChild(QString tagName, QSharedPointer<Asset> child)
{
	child->setSelfPtr(child);
	child->setParentAsset(selfPtr());
	children_[tagName].push_back(child);
	connect(child.data(),SIGNAL(edited()),this,SLOT(childEdited()));
}

QList<QSharedPointer<Asset>> DataStore::getGeneratedChildren(QString tagName)
{
	if(!children_.contains(tagName))
		return QList<QSharedPointer<Asset>>();
	return children_[tagName];
}

QStringList DataStore::getValidChildTags()
{
	return factories_.keys();
}

QSharedPointer<AssetFactory> DataStore::getAssetFactory(QString tagName)
{
	if(factories_.contains(tagName))
		return factories_[tagName];
	return QSharedPointer<AssetFactory>();
}
QSharedPointer<Asset> DataStore::createChildAsset(QString tagName,QString type,QString& error)
{
	QSharedPointer<Asset> newAsset;
	if(tagName == "")
		return newAsset;
	QSharedPointer<AssetFactory> factory = getAssetFactory(tagName);
	if(factory.isNull())
		return newAsset;
	newAsset = factory->getAsset(error,type);
	if(newAsset.isNull())
		return newAsset;

	AddChild(tagName,newAsset);
	//Deserialize the child into a default state.
	newAsset->initializeToDefault(tagName,type);
	newAsset->setEdited();
	return newAsset;
}

void DataStore::clear()
{
	propertyContainer_->clear();
	//When we deserialize again, the datastore will be effectively cleared.  To
	//clear it for serialization, just set all children to deleted, and none of
	//them will be serialized out.
	foreach(QList<QSharedPointer<Asset>> assetList,children_)
	{
		foreach(QSharedPointer<Asset> asset,assetList)
		{
			asset->setDeleted();
		}
	}
}

void DataStore::setPropertyRuntimeEditable(QString propName, bool editable)
{
	propertyContainer_->getProperty(propName)->setRuntimeEditable(editable);
}

QList<QSharedPointer<DataStore>> DataStore::getRuntimeEditableDescendants()
{
	QList<QSharedPointer<DataStore>> runtimeDesc;
	if(isRuntimeEditable())
		runtimeDesc.append(selfPtr().staticCast<DataStore>());
		
	QStringList childTags = getDefinedChildTags();
	QList<QSharedPointer<Asset>> childList;
	foreach(QString childTag,childTags)
	{
		childList = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> child,childList)
		{
			if(child->inherits("Picto::DataStore"))
			{
				runtimeDesc.append(child.staticCast<DataStore>()->getRuntimeEditableDescendants());
			}
		}
	}
	return runtimeDesc;
}
QList<QSharedPointer<Property>> DataStore::getDescendantsProperties()
{
	QList<QSharedPointer<Property>> descendantProps;
	QHash<QString, QVector<QSharedPointer<Property>>> propMap = propertyContainer_->getProperties();
	foreach(QVector<QSharedPointer<Property>> propVec, propMap)
	{
		foreach(QSharedPointer<Property> prop, propVec)
		{
			descendantProps.append(prop);
		}
	}

	QStringList childTags = getDefinedChildTags();
	QList<QSharedPointer<Asset>> childList;
	foreach(QString childTag,childTags)
	{
		childList = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> child,childList)
		{
			if(child->inherits("Picto::DataStore"))
			{
				descendantProps.append(child.staticCast<DataStore>()->getDescendantsProperties());
			}
		}
	}
	return descendantProps;
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
