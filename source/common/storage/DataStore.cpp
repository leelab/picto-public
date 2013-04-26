#include <QVariant>

#include "DataStore.h"
#include "../property/Property.h"
#include "PropertyFactory.h"
#include "../parameter/AssociateElement.h"
#include "../memleakdetect.h"

namespace Picto {

DataStore::DataStore():
Asset()
{
	assetId_ = 0;
	propertyContainer_ = PropertyContainer::create("DataStore");
	connect(this,SIGNAL(deleted()),this,SIGNAL(edited()));
}


DataStore::~DataStore()
{
	int noOp=0;
	noOp++;
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

	//Check if the token includes an id and if that id has changed
	if(xmlReader->attributes().hasAttribute("id"))
	{
		//Check if the id value has changed
		if(xmlReader->attributes().value("id").toString().toInt()!=assetId_)
		{
			xmlStreamWriter->writeStartElement(xmlReader->name().toString());
			foreach(QXmlStreamAttribute attribute,xmlReader->attributes())
			{
				if(attribute.name() == "id")
					continue;
				xmlStreamWriter->writeAttribute(attribute.name().toString(),attribute.value().toString());
			}
			//Write the new Id.
			xmlStreamWriter->writeAttribute("id",QString::number(assetId_));
		}
		else
		{
			//Nothing changed.  Just write out old value.
			xmlStreamWriter->writeCurrentToken(*xmlReader);
		}
	}
	else
	{
		//There is no id value.  Add it.
		xmlStreamWriter->writeCurrentToken(*xmlReader);
		xmlStreamWriter->writeAttribute("id",QString::number(assetId_));
	}

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
		addError(myTagName_.toLatin1(),QString("Incorrect tag, expected <%1>").arg(myTagName_).toLatin1(),xmlStreamReader);
		returnVal = false;
		return returnVal;
	}


	foreach(QXmlStreamAttribute attribute,xmlStreamReader->attributes())
	{
		if(attribute.name() == "id")
		{
			assetId_ = attribute.value().toString().toInt();
		}
		else if(attribute.name() == "type")
		{
		}
		else
		{
			//Make sure that there are no attributes besides "type" and "id"
			addError(myTagName_.toLatin1(),QString("Incorrect attribute:\"%1\", only \"type\" and \"id\" attributes are allowed").arg(attribute.name().toString()),xmlStreamReader);
			returnVal = false;
			return returnVal;
		}
	}

	////Make sure that there are no attributes besides "type"
	//int numAttributes = xmlStreamReader->attributes().size();
	//if((numAttributes > 1) || ((numAttributes == 1) && !xmlStreamReader->attributes().hasAttribute("type")))
	//{
	//	addError(myTagName_.toLatin1(),QString("Incorrect attribute(s), only \"type\" attributes are allowed"),xmlStreamReader);
	//	returnVal = false;
	//	return returnVal;
	//}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	bool childWithSameTag = false;	//If this is true, we just deserialized a child with the same tag, so we should ignore its end tag.
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == myTagName_) && !childWithSameTag) && !xmlStreamReader->atEnd())
	{
		childWithSameTag = false;
		//Move forward in XMLStream
		if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
		{
			addError(myTagName_.toLatin1(),QString("XML syntax has been violated.").toLatin1(),xmlStreamReader);
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
			addError(myTagName_.toLatin1(),QString("The tag: <%1> is invalid in the %2 object definition").arg(name).arg(myTagName_).toLatin1(),xmlStreamReader);
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
			Q_ASSERT_X(!childWithSameTag,"DataStore::deserializeFromXml",QString("The tag: <%1> has the same name as its parent and cannot be used as a placeholder").arg(name).toLatin1());
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
			addError(myTagName_.toLatin1(),QString(error),xmlStreamReader);
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
		addError(myTagName_.toLatin1(), "Unexpected end of document", xmlStreamReader);
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
	foreach(QUuid key,associateChildrenByGuid_.keys())
	{
		QMap<QString,QList<QSharedPointer<Asset>>> associateChildren = associateChildrenByGuid_.value(key);
		foreach(QList<QSharedPointer<Asset>> childList,associateChildren)
		{
			foreach(QSharedPointer<Asset> child,childList)
			{
				returnVal &= child->validateTree();
			}
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
	foreach(QUuid key,associateChildrenByGuid_.keys())
	{
		QMap<QString,QList<QSharedPointer<Asset>>> associateChildren = associateChildrenByGuid_.value(key);
		foreach(QList<QSharedPointer<Asset>> AssetList,associateChildren)
		{
			foreach(QSharedPointer<Asset> child,AssetList)
			{
				child->setDeleted();
			}
		}
	}
}

void DataStore::postDeserialize()
{
	Asset::postDeserialize();
}

bool DataStore::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QHash<QString,bool> nameMap;
	QString name;
	//First make sure that the experimental children all have different names
	foreach(QList<QSharedPointer<Asset>> assetList,children_.values())
	{
		foreach(QSharedPointer<Asset> child,assetList)
		{
			if(child->needsUniqueName())
			{
				name = child->getName();
				if(nameMap.contains(name))
				{
					QString errMsg = QString("%1 contains more than one child with the name: %2").arg(getName()).arg(name);
					addError("DataStore", errMsg, xmlStreamReader);
					return false;
				}
				nameMap[name] = true;
			}
		}
	}
	//Now check that each individual Associate child has a different name
	foreach(QUuid key,associateChildrenByGuid_.keys())
	{
		QMap<QString,QList<QSharedPointer<Asset>>> associateChildren = associateChildrenByGuid_.value(key);
		QMap<QString,bool> associateNameMap;
		foreach(QList<QSharedPointer<Asset>> assetList,associateChildren.values())
		{
			foreach(QSharedPointer<Asset> child,assetList)
			{
				if(child->needsUniqueName())
				{
					name = child->getName();
					if(nameMap.contains(name))
					{
						QString errMsg = QString("%1 contains experimental and analysis elements with the same name: %2").arg(getName()).arg(name);
						addError("DataStore", errMsg, xmlStreamReader);
						return false;
					}
					if(associateNameMap.contains(name))
					{
						QString errMsg = QString("%1 more than one analysis element child with the same name: %2").arg(getName()).arg(name);
						addError("DataStore", errMsg, xmlStreamReader);
						return false;
					}
					associateNameMap[name] = true;
				}
			}
		}
	}
	return true;
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
	bool factoryOverride = factories_.contains(tagName);
	if(factoryOverride)
	{	//A previously created factory is being overriden.  If that factory was a 
		//property, we need to remove it from the orderedPropList_;
		for(int i=orderedPropList_.size()-1;i>=0;i--)
		{
			if(orderedPropList_[i] == tagName)
			{
				orderedPropList_.removeAt(i);
			}
		}
	}
	factories_[tagName]=factory;
}

void DataStore::AddChild(QString tagName, QSharedPointer<Asset> child)
{
	child->setSelfPtr(child);
	child->setParentAsset(selfPtr());
	children_[tagName].push_back(child);
	connect(child.data(),SIGNAL(edited()),this,SLOT(childEdited()));
}

bool DataStore::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(searchRequest.type == SearchRequest::EXISTS)
		return true;
	return false;
}

bool DataStore::AddAssociateChild(QUuid associateId, int parentId, QString tagName, QSharedPointer<Asset> child)\
{
	//If the parent id matches mine, add the associate child
	if(parentId == getAssetId())
	{
		AddAssociateChild(associateId,tagName,child);
		return true;	//The child has been added, return true.
	}
	//Recursively attempt to add the asset to descendants.
	foreach(QList<QSharedPointer<Asset>> assetList,children_)
	{
		foreach(QSharedPointer<Asset> asset,assetList)
		{
			if(asset->inherits("Picto::DataStore"))
			{
				if(asset.staticCast<DataStore>()->AddAssociateChild(associateId,parentId,tagName,child))
					return true;	//A descendant added the child
			}
		}
	}
	//No descendant contained the input asset id.  The child was not added.
	return false;
}

bool DataStore::AddAssociateChild(QUuid associateId, QString parentPath, QString tagName, QSharedPointer<Asset> child)
{
	//If the parent id matches mine, add the associate child
	if(parentPath == getPath())
	{
		AddAssociateChild(associateId,tagName,child);
		return true;	//The child has been added, return true.
	}
	//Recursively attempt to add the asset to descendants.
	foreach(QList<QSharedPointer<Asset>> assetList,children_)
	{
		foreach(QSharedPointer<Asset> asset,assetList)
		{
			if(asset->inherits("Picto::DataStore"))
			{
				if(asset.staticCast<DataStore>()->AddAssociateChild(associateId,parentPath,tagName,child))
					return true;	//A descendant added the child
			}
		}
	}
	//No descendant contained the input asset id.  The child was not added.
	return false;
}

void DataStore::AddAssociateChild(QUuid associateId, QString tagName, QSharedPointer<Asset> child)
{
	associateChildrenByGuid_[associateId][tagName].push_back(child);
	AssociateElement* assocElement = dynamic_cast<AssociateElement*>(child.data());
	Q_ASSERT(assocElement);
	assocElement->linkToAsset(selfPtr());
	emit childAddedAfterDeserialize(child);
	connect(child.data(),SIGNAL(edited()),this,SIGNAL(associateChildEdited()));
}
void DataStore::ClearAssociateDescendants(QUuid associateId)
{
	ClearAssociateChildren(associateId);
	//Recursively tell each child to clear its associate descendants.
	foreach(QList<QSharedPointer<Asset>> assetList,children_)
	{
		foreach(QSharedPointer<Asset> asset,assetList)
		{
			if(asset->inherits("Picto::DataStore"))
			{
				asset.staticCast<DataStore>()->ClearAssociateDescendants(associateId);
			}
		}
	}
}

void DataStore::ClearAllAssociateDescendants()
{
	foreach(QUuid associateId,associateChildrenByGuid_.keys())
	{
		ClearAssociateDescendants(associateId);
	}
}

void DataStore::ClearAssociateChildren(QUuid associateId)
{
	if(!associateChildrenByGuid_.contains(associateId))
		return;
	//Disconnect all edited() signals
	foreach(QList<QSharedPointer<Asset>> associateList,associateChildrenByGuid_[associateId].values())
	{
		foreach(QSharedPointer<Asset> associate,associateList)
		{
			disconnect(associate.data(),SIGNAL(edited()),this,SIGNAL(associateChildEdited()));
		}
	}
	//Clear the associateId submap from the associateMap
	associateChildrenByGuid_.remove(associateId);
}

void DataStore::ClearAllAssociateChildren()
{
	foreach(QUuid associateId,associateChildrenByGuid_.keys())
	{
		ClearAssociateChildren(associateId);
	}
}

void DataStore::setDescendantPropertiesAsAssociates()
{
	propertyContainer_->setPropertiesAsAssociates(true);
	foreach(QList<QSharedPointer<Asset>> assetList,children_)
	{
		foreach(QSharedPointer<Asset> asset,assetList)
		{
			if(asset->inherits("Picto::DataStore"))
			{
				asset.staticCast<DataStore>()->setDescendantPropertiesAsAssociates();
			}
		}
	}
}

QList<QSharedPointer<Asset>> DataStore::getGeneratedChildren(QString tagName)
{
	if(!children_.contains(tagName))
		return QList<QSharedPointer<Asset>>();
	return children_[tagName];
}

QList<QSharedPointer<Asset>> DataStore::getAssociateChildren(QUuid associateId, QString tagName)
{
	if(!associateChildrenByGuid_.contains(associateId) || !associateChildrenByGuid_.value(associateId).contains(tagName))
		return QList<QSharedPointer<Asset>>();
	return associateChildrenByGuid_[associateId][tagName];
}

QList<QUuid> DataStore::getAttachedAssociateIds()
{
	return associateChildrenByGuid_.keys();
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

	//Add the child to this asset
	AddChild(tagName,newAsset);

	//Deserialize the child into a default state.
	newAsset->initializeToDefault(tagName,type);

	//Tell everyone that this was added now that its in an initialized state.
	emit childAddedAfterDeserialize(newAsset);

	//Set this asset as edited (since the child was added)
	setEdited();
	return newAsset;
}

QSharedPointer<Asset> DataStore::importChildAsset(QString childXml,QString& error)
{
	//Get tag name and type from childXML
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(childXml));
	while(!xmlStreamReader->isStartElement() && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
	}
	if(xmlStreamReader->atEnd())
		return QSharedPointer<Asset>();
	QString tagName = xmlStreamReader->name().toString();
	if(tagName.isEmpty())
		return QSharedPointer<Asset>();
	QString type = "";
	if(xmlStreamReader->attributes().hasAttribute("type"))
		type = xmlStreamReader->attributes().value("type").toString();

	//Create the new asset from one of this object's factories
	QSharedPointer<AssetFactory> factory = getAssetFactory(tagName);
	if(factory.isNull())
		return QSharedPointer<Asset>();
	QSharedPointer<Asset> newAsset = factory->getAsset(error,type);
	if(newAsset.isNull())
		return QSharedPointer<Asset>();

	//Add the child to this asset
	AddChild(tagName,newAsset);

	//Initialize the child from xml
	bool success = newAsset->initializeFromXml(childXml);
	if(!success)
	{
		newAsset->setDeleted();
		return QSharedPointer<Asset>();
	}

	//Tell everyone that this was added now that its been deserialized and is ready.
	emit childAddedAfterDeserialize(newAsset);

	//Set this asset as edited (since the child was added)
	setEdited();
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
	ClearAllAssociateChildren();
}

void DataStore::setPropertyRuntimeEditable(QString propName, bool editable)
{
	propertyContainer_->getProperty(propName)->setRuntimeEditable(editable);
}

void DataStore::enableRunModeForDescendants(bool en)
{
	enableRunMode(en);
	QStringList childTags = getDefinedChildTags();
	QList<QSharedPointer<Asset>> childList;
	foreach(QString childTag,childTags)
	{
		childList = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> child,childList)
		{
			if(child->inherits("Picto::DataStore"))
				child.staticCast<DataStore>()->enableRunModeForDescendants(en);
			else
				child->enableRunMode(en);
		}
	}
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

int DataStore::getAssetId()
{
	return assetId_;
}
void DataStore::setAssetId(int id)
{
	assetId_ = id;
	emit assetIdEdited();
}

void DataStore::upgradeVersion(QString deserializedVersion)
{
	QMap<QString,QList<QSharedPointer<Asset>>> deserializedChildren = children_;
	Asset::upgradeVersion(deserializedVersion);
	foreach(QList<QSharedPointer<Asset>> AssetList,deserializedChildren)
	{
		foreach(QSharedPointer<Asset> child,AssetList)
		{
			child->upgradeVersion(deserializedVersion);
		}
	}
}

//Returns true if this object is part of the requested search (ie. It falls into the searchRequest's group
bool DataStore::isPartOfSearch(SearchRequest searchRequest)
{
	if(searchRequest.group & SearchRequest::EXPERIMENT)
		return true;
	return false;
}

bool DataStore::searchForQuery(SearchRequest searchRequest)
{
	if(!isPartOfSearch(searchRequest))
		return false;
	return executeSearchAlgorithm(searchRequest);
}
bool DataStore::searchRecursivelyForQuery(SearchRequest searchRequest)
{
	if(!searchRequest.enabled)
		return false;
	if(searchForQuery(searchRequest))
		return true;
	if(searchChildrenRecursivelyForQuery(searchRequest))
		return true;
	return false;
}
bool DataStore::searchChildrenForQuery(SearchRequest searchRequest)
{
	if(!searchRequest.enabled)
		return false;

	//Call searchForQuery on all descendants 
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			if(tagChild->inherits("Picto::DataStore"))
			{
				if(tagChild.staticCast<DataStore>()->searchForQuery(searchRequest))
					return true;
			}
		}
	}
	//Call searchForQuery on all attached associates
	foreach(QUuid associateId,associateChildrenByGuid_.keys())
	{
		foreach(QList<QSharedPointer<Asset>> associateList,associateChildrenByGuid_[associateId].values())
		{
			foreach(QSharedPointer<Asset> associate,associateList)
			{
				if(associate.staticCast<DataStore>()->searchForQuery(searchRequest))
					return true;
			}
		}
	}
	return false;
}
bool DataStore::searchChildrenRecursivelyForQuery(SearchRequest searchRequest)
{
	if(!searchRequest.enabled)
		return false;
	//Call searchRecursivelyForQuery on all descendants 
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			if(tagChild->inherits("Picto::DataStore"))
			{
				if(tagChild.staticCast<DataStore>()->searchRecursivelyForQuery(searchRequest))
					return true;
			}
		}
	}
	//Call searchRecursivelyForQuery on all attached associates
	foreach(QUuid associateId,associateChildrenByGuid_.keys())
	{
		foreach(QList<QSharedPointer<Asset>> associateList,associateChildrenByGuid_[associateId].values())
		{
			foreach(QSharedPointer<Asset> associate,associateList)
			{
				if(associate.staticCast<DataStore>()->searchForQuery(searchRequest))
					return true;
			}
		}
	}
	return false;
}

bool DataStore::searchParentForQuery(SearchRequest searchRequest)
{
	if(!searchRequest.enabled)
		return false;
	QSharedPointer<Asset> parentAsset = getParentAsset();
	if(parentAsset.isNull())
		return false;
	QSharedPointer<DataStore> parentDataStore = parentAsset.dynamicCast<DataStore>();
	if(parentDataStore.isNull())
		return false;
	//Check the parent asset for the query
	if(parentDataStore->searchForQuery(searchRequest))
		return true;
	//Check the parent asset's children for the query
	if(parentDataStore->searchChildrenForQuery(searchRequest))
		return true;
	return false;
}
bool DataStore::searchAncestorsForQuery(SearchRequest searchRequest)
{
	if(!searchRequest.enabled)
		return false;
	//Check my parent for the query
	if(searchParentForQuery(searchRequest))
		return true;

	//Check more distant ancestors for the query
	QSharedPointer<Asset> parentAsset = getParentAsset();
	if(parentAsset.isNull())
		return false;
	QSharedPointer<DataStore> parentDataStore = parentAsset.dynamicCast<DataStore>();
	if(parentDataStore.isNull())
		return false;
	if(parentDataStore->searchAncestorsForQuery(searchRequest))
		return true;
	return false;
}

void DataStore::childEdited()
{
	emit edited();
}

}; //namespace Picto
