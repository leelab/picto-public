#include <QVariant>

#include "DataStore.h"
#include "../property/Property.h"
#include "PropertyFactory.h"
#include "../parameter/AssociateElement.h"
#include "../memleakdetect.h"

namespace Picto {

DataStore::DataStore():
Asset(),
needsUniqueChildNames_(true)
{
	assetId_ = 0;
	propertyContainer_ = PropertyContainer::create("DataStore");
	//If this Datastore was deleted, then it was also edited.
	connect(this,SIGNAL(deleted()),this,SIGNAL(edited()));
}


DataStore::~DataStore()
{
	int noOp=0;
	noOp++;
}
//Autoserialization Stuff-------------------------------------------------------------

/*! \brief Serializes this DataStore along with descendants into a descriptive XML string using the input QXmlStreamWriter.
 *	\details The serialization algorithm in Picto is fairly complex and took some time to develop,
 *	so think twice before changing it.  One of the sources for the complexity, however, was the
 *	desire for the XML code to look the same apart from a small change if we edit the design in
 *	software and then reserialize it out.  We wanted to be able to maintain XML comments without
 *	their getting deleted and to be sure that the order of the XML tags that was put into 
 *	the deserializeFromXml() function would be the same as the order that was put out of the 
 *	serializeAsXml() function.  These requirements are less important now than they once were since
 *	we used to include an XML editor in the Workstation that allowed users to directly manipulate 
 *	Design XML.  Now that we no longer use that, it is still nice to maintain these restrictions 
 *	for the purposes of using file comparison tools to compare different versions of an experiment.
 *	On the other hand, since no one ever touches the XML manually, things like maintaining
 *	ordering in multiple versions of the code might come for free.  While you might consider
 *	relaxing these restrictions in the future and thereby simplifying this function, an overview of 
 *	its operation right now is as follows:
 *		-# Start reading out the XML code that was saved during the course of deserializeFromXml().
 *			This code includes the tag names of this DataStore and its immediate children.
 *		-# Write out this DataStores tag name, updating the ID value if there is none or our 
 *			AssetID has changed.
 *		-# Loop through the XML code saved on input.  
 *			- Each time a tag is encountered, move to 
 *				the first child of its type and serialize it out. (If it was deleted, nothing will 
 *				happen).
 *			- If no child type matches the tag, the tag must be an XML comment, write it out.
 *			- Continue the loop.
 *		-# Loop through any child assets that were not yet serialized out.
 *			- If the asset is both new and edited, write it out.
 *			- Otherwise, skip the asset.  It was either new and undedited: a default asset 
 *				(required and initialized to a default state), or it was edited and not new: 
 *				a complicated condition discussed in comments below.
 *				Only the new, edited version should be serialized.
 *		-# Write out this DataStore's closing tag.
 */
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

/*! \brief Deserializes this DataStore along with descendants from the descriptive XML string input 
*	in the QXmlStreamReader where the read pointer is at this DataStore's start tag.
 *	\details The serialization/deserialization algorithm in Picto is fairly complex and took some time to develop,
 *	so think twice before changing it.  One of the sources for the complexity, however, was the
 *	desire for the XML code to look the same apart from a small change if we edit the design in
 *	software and then reserialize it out.  We wanted to be able to maintain XML comments without
 *	their getting deleted and to be sure that the order of the XML tags that was put into 
 *	the deserializeFromXml() function would be the same as the order that was put out of the 
 *	serializeAsXml() function.  These requirements are less important now than they once were since
 *	we used to include an XML editor in the Workstation that allowed users to directly manipulate 
 *	Design XML.  Now that we no longer use that, it is still nice to maintain these restrictions 
 *	for the purposes of using file comparison tools to compare different versions of an experiment.
 *	On the other hand, since no one ever touches the XML manually, things like maintaining
 *	ordering in multiple versions of the code might come for free.  While you might consider
 *	relaxing these restrictions in the future and thereby simplifying this function, an overview of 
 *	its operation right now is as follows:
 *		-# Create a QXMLStreamWriter to write each tag to a stored tagText_ string as we deserialize it.
 *			Note that since we only ever look at the top level tag names of children, this will
 *			will only contain this DataStore's tag and each immediate child tag.
 *		-# Store this DataStores tag name, (since it may be different from the default tag name)
 *			and store its AssetID from the 'id' tag property.
 *		-# Loop through the input XML code until we reach the closing tag for this DataStore.
 *			- Write everything that we read in to our stored tagText_ string
 *			- Each time a start tag is encountered, try to create a child asset by looking for
 *				an AssetFactory in our factories_ lookup table under the start tag's name.
 *			- If we find the asset factory, try to create an Asset using the factory according
 *				to the start tag's 'type' property.
 *			- If we successfully created the child asset, add it to our children lists and
 *				deserialize it, passing in the input QXMLStreamReader.
 *			- When deserialization is complete, the QXMLStreamReader's read pointer will be at
 *				the child assets closing tag.  Write the closing tag to our tagText_ string.
 *		-# If any AssetFactory objects in our factories_ lookup require us to generate more 
 *			children that were already generated, generate them from those factories, add
 *			them to the children lists and initialize them to their default states.
 *		-# Write this DataStore's closing tag to the tagText_ string.
 *	\sa AssetFactory, AddDefinableObjectFactory(), AddDefinableProperty(), AddChild(), 
 *	\sa Asset::initializeToDefault()
 */
bool DataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
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
		addError(QString("Incorrect tag, expected <%1>").arg(myTagName_).toLatin1());
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
			addError(QString("Incorrect attribute:\"%1\", only \"type\" and \"id\" attributes are allowed").arg(attribute.name().toString()));
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
			addError(QString("XML syntax has been violated.").toLatin1());
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
			addError(QString("The tag: <%1> is invalid in the %2 object definition").arg(name).arg(myTagName_).toLatin1());
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
			addError(QString(error));
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
		if(!newChild->fromXml(xmlStreamReader))
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
		addError("Unexpected end of document");
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

/*! \brief Returns true if this asset and all of its children have valid syntax.
 *	\details Calls validateTree on all immediate children, then all Associate Children,
 *	then calls ValidateObject() on this DataStore.  If any of them return a false
 *	value for valid syntax, this function returns false, otherwise it returns true.
 *	\note Regardless of what this function returns, all children, associate children
 *	and this object will have their validity checked such that we can expect all of them
 *	to report errors that can be read from Serializable::getErrors().  Validation doesn't
 *	stop after the first error.
 */
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

/*! \brief Emits the deleted() signal when called to inform interested parties that this asset needs to be removed from the design.  Calls setDeleted() on all children and associate children as well.
 *	\details Since internally we connect deleted() to receivedDeletedSignal(), it also causes our deleted_ flag to be set so
 *	that this object itself will know if it has been deleted_.
 *	\note Currently in Picto, in order to delete an asset we just set it as deleted, then when the time comes for serialization,
 *	we don't serialize out any of the "delete marked" assets.  Doing things this way helps us maintain a general order and structure in
 *	xml design files from one serialization to the next to aid in comparison of different versions of a design file.
 *	\attention Probably for the sake of future proofing, instead of directly emitting deleted(), this function should call
 *	Asset::setDeleted() and let it do it.  I am not changing this now, but you probably should.
 * \sa serializeAsXml()
 */
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

/*! \copydoc Asset::validateObject()
 *	\details In the case of the DataStore, this function checks for uniqueness of child names when
 *	needsUniqueChildNames() and any child or associate child needsUniqueName()
 */
bool DataStore::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QHash<QString,bool> nameMap;
	QString name;
	//If necessary check to see if children have unique names
	if(needsUniqueChildNames())
	{
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
						addError(errMsg);
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
							addError(errMsg);
							return false;
						}
						if(associateNameMap.contains(name))
						{
							QString errMsg = QString("%1 contains more than one analysis element child with the same name: %2").arg(getName()).arg(name);
							addError( errMsg);
							return false;
						}
						associateNameMap[name] = true;
					}
				}
			}
		}
	}
	return true;
}

/*! \brief In theory this just initializes all properties to default values.  In practice it just calls initializeToDefaults() to initialize the entire DataStore to its default values.
*/
void DataStore::initializePropertiesToDefaults()
{
	initializeToDefault();
}

/*! \brief Adds a required property to this DataStore with the input name and defaultValue of type defined by the input default value QVariant type.
 *	\sa AddDefinableProperty(int,QString,QVariant,QMap<QString,QVariant>,int,int)
 */
void DataStore::AddDefinableProperty(
		QString tagName, 
		QVariant defaultValue
		)
{
	AddDefinableProperty(defaultValue.type(),tagName,defaultValue,QMap<QString,QVariant>(),1,1);
}
/*! \brief Adds a required property to this DataStore of the input type (a QVariant::Type) with the input name and defaultValue, plus a single 
 *attribute with input singleAttributeName and singleAttributeValue
 *	\note I don't think we ever use this function because if we require any attributes (ie. EnumProperty)
 *	then we require more than one.
 *	\sa AddDefinableProperty(int,QString,QVariant,QMap<QString,QVariant>,int,int)
 */
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

/*! \brief Sets property generation rules for a property with the input type and tagName to this DataStore 
 *	\details type distinguishes the type of property (ie. QVarant::Int, QVariant::Bool, QVariant::String), 
 *	tagName is the name of this property, defaultValue is the value
 *	that the property recieves automatically on generation, attributeMap is a map of attributes that will be
 *	provided to the generated property (some properties require this like EnumProperty require this),
 *	minNumOfThisType is the required number of properties of this kind (we can be sure that at least
 *	this number of properties of this kind will be generated), maxNumOfThisType is the maximum allowable
 *	number of properties of this type.
 *	Typically, a DataStore calls some variant of AddDefinableProperty() severral times in its constructor 
 *	to define the types of properties that it will include.  The order in which AddDefinableProperty() is
 *	called defines the order in which properties will appear in the UI.
 *	\note Internally, this function uses AddDefinableObjectFactory() to add a PropertyFactory to this
 *	data store, so you can look at that function for more detail.
 *	\attention In practice, we never use more than one property of a particular type, tagName combination.
 *	In fact, if we ever used the same tagName for more than one property, it would probably cause a lot 
 *	of things to break since we sometimes use property paths to identify properties, and adding two 
 *	properties of the same name would cause two properties to have the same path.  The point is that this
 *	function provides generality that should not actually be used.  Possibly we should just get rid of 
 *  this generalization and remove the maxNumOfThisType parameter.
 *	\sa AddDefinableObjectFactory(), PropertyFactory
 */
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

/*! \brief Adds an AssetFactory to this DataStore for the input tagName XML tag.
 *	\details This function adds the input AssetFactory to a lookup table.  When the deserialization 
 *	system encounters an xml tag with the input tagName, the deserialization system uses it to find
 *	this AssetFactory and uses the factory to generate an Asset.  Since it was looked up with
 *	the corresponding XML tag, this Asset will be of the correct kind to match the XML tag's contents
 *	and so the XML tag can be used to deserialize the newly generated Asset.  The various parameters
 *	of the AssetFactory offer additional control including generating sub Asset types based on the 
 *	XML tag's type property and enforcing minimum and maximum numbers of generated Assets for each
 *	DataStore.
 */
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

/*! \brief Adds the input child Asset to this DataStore's child list, under the tagName category (where tagName will also be the XML tag used to serialize the child)
 *	\details This function provides the child with its shared pointer selfPtr, tells it who its parent is,
 *	adds it to this DataStore's children_ lookup table under the tagName category, and connects up the
 *	childs edited() signal to this DataStore's childEdited() slot.
 *	\sa AddAssociateChild()
 */
void DataStore::AddChild(QString tagName, QSharedPointer<Asset> child)
{
	child->setSelfPtr(child);
	child->setParentAsset(selfPtr());
	children_[tagName].push_back(child);
	connect(child.data(),SIGNAL(edited()),this,SLOT(childEdited()));
}

/*! \brief Returns true if this DataStore contains the input SearchRequest in one of its searchable property fields
 *	\sa SearchRequest
 */
bool DataStore::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(searchRequest.type == SearchRequest::EXISTS)
		return true;
	return false;
}

/*! \brief Recursively attempts to add the input child Asset to this DataStore as an AssociateElement child under the tagName category for the AssociateRoot
 *	with the input associateId.
 *	\details If the parentId matches this DataStore's AssetId, it is added.  Otherwise, child elements' AddAssociateChild()
 *	functions are called until on of them successfully adds the AssociateElement child.  If a match cannot be found for the
 *	parentId, false is returned.  If a match is found, the function returns true.
 *	\sa AddChild(), AddAssociateChild(QUuid, QString, QSharedPointer<Asset>)
 */
bool DataStore::AddAssociateChild(QUuid associateId, int parentId, QString tagName, QSharedPointer<Asset> child)
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

/*! \brief Recursively attempts to add the input child Asset to this DataStore as an AssociateElement child under the tagName category for the AssociateRoot
 *	with the input associateId.
 *	\details If the parentPath matches this DataStore's path, it is added.  Otherwise, child elements' AddAssociateChild()
 *	functions are called until on of them successfully adds the AssociateElement child.  If a match cannot be found for the
 *	parentPath, false is returned.  If a match is found, the function returns true.
 *	\attention This function can be more efficient by checking if the parentPath starts with this DataStore's path and 
 *	returning false if not.  If you have a few minutes, you can make this change.
 *	\sa AddChild(), AddAssociateChild(QUuid, QString, QSharedPointer<Asset>)
 */
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

/*! \brief Adds the input child Asset to this DataStore as an AssociateElement child under the tagName category for the AssociateRoot
 *	with the input associateId.
 *	\details The child is added to a lookup table of lookup tables, under its associateId and tagName.  It's 
 *	AssociateElement::linkToAsset() function is called to link it to this DataStore, its edited() signal is connected
 *	to our associateChildEdited() slot, and childAddedAfterDeserialize() is emitted to let interested parties know what
 *	happened.
 *	\sa AddChild(), AssociateElement::linkToAsset()
 */
void DataStore::AddAssociateChild(QUuid associateId, QString tagName, QSharedPointer<Asset> child)
{
	associateChildrenByGuid_[associateId][tagName].push_back(child);
	AssociateElement* assocElement = dynamic_cast<AssociateElement*>(child.data());
	Q_ASSERT(assocElement);
	assocElement->linkToAsset(selfPtr());
	emit childAddedAfterDeserialize(child);
	connect(child.data(),SIGNAL(edited()),this,SIGNAL(associateChildEdited()));
}

/*! \brief Removes all AssociateElement children with the input associateId from this DataStore and all of this DataStore's descendants.
 */
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

/*! \brief Removes all AssociateElement children from this DataStore and all of this DataStore's descendants.
 */
void DataStore::ClearAllAssociateDescendants()
{
	foreach(QUuid associateId,associateChildrenByGuid_.keys())
	{
		ClearAssociateDescendants(associateId);
	}
}

/*! \brief Removes all AssociateElement children with the input associateId from this DataStore.
 */
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

/*! \brief Removes all AssociateElement children from this DataStore.
 */
void DataStore::ClearAllAssociateChildren()
{
	foreach(QUuid associateId,associateChildrenByGuid_.keys())
	{
		ClearAssociateChildren(associateId);
	}
}

/*! \brief Sets properties of this DataStore and all of its descendants as Associates
 *	\details Picto doesn't contain an AssociateProperty class, but we need to know sometimes if a Property belongs
 *	to an experimental element or an AssociateElement in order to be able to tell if it is necessary in an experimental
 *	run, etc.  For this reason we simply maintain a flag on each Property that stores whether it is an Associate or not.
 *	This function sets all flags of all Property objects of all of its descendants to true
 *	\sa Property::setAssociateProperty()
 */
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

/*! \brief Returns a list of all child Assets that were generated under the input tagName XML tag / category
 *	\sa deserializeFromXml(), AddChild()
 */
QList<QSharedPointer<Asset>> DataStore::getGeneratedChildren(QString tagName)
{
	if(!children_.contains(tagName))
		return QList<QSharedPointer<Asset>>();
	return children_[tagName];
}

/*! \brief Returns a list of all XML tags / categories under which AssociateElement children were added to this DataStore for the input AssociateRoot associateId.
 *	\sa AddAssociateChild()
 */
QStringList DataStore::getAssociateChildTags(QUuid associateId)
{
	if(!associateChildrenByGuid_.contains(associateId))
		return QStringList();
	return associateChildrenByGuid_.value(associateId).keys();
}

/*! \brief Returns a list of all Associate Element child Assets that were added under the input XML tag/category for the input AssociateRoot associateId.
 *	\sa AddAssociateChild()
 */
QList<QSharedPointer<Asset>> DataStore::getAssociateChildren(QUuid associateId, QString tagName)
{
	if(!associateChildrenByGuid_.contains(associateId) || !associateChildrenByGuid_.value(associateId).contains(tagName))
		return QList<QSharedPointer<Asset>>();
	return associateChildrenByGuid_[associateId][tagName];
}

/*! \brief Returns a list of all Associate Element Assets that were added to this DataStore and all of its descendants for the input AssociateRoot associateId.
 *	\details Returned values are according to a Pre-Ordered Depth-First-Search
 *	\sa AddAssociateChild()
 */
QList<QSharedPointer<Asset>> DataStore::getAssociateDescendants(QUuid associateId)
{
	QList<QSharedPointer<Asset>> returnVal;
	foreach(QString associateTag,getAssociateChildTags(associateId))
	{
		returnVal.append(getAssociateChildren(associateId,associateTag));
	}
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			if(tagChild->inherits("Picto::DataStore"))
			{
				returnVal.append(tagChild.staticCast<DataStore>()->getAssociateDescendants(associateId));
			}
		}
	}
	return returnVal;
}

/*! \brief Returns a list of all AssociateRoot AssociateId's for which Associate Element children have been added to this DataStore.
 */
QList<QUuid> DataStore::getAttachedAssociateIds()
{
	return associateChildrenByGuid_.keys();
}

/*! \brief Returns a list of all XML tags for which children have been or can be added to this DataStore.
 */
QStringList DataStore::getValidChildTags()
{
	return factories_.keys();
}

/*! \brief Returns the AssetFactory used by this DataStore to handle XML tags with the input tagName
*/
QSharedPointer<AssetFactory> DataStore::getAssetFactory(QString tagName)
{
	if(factories_.contains(tagName))
		return factories_[tagName];
	return QSharedPointer<AssetFactory>();
}
/*! \brief Creates a child asset with the input XML tag name and type property and returns it.
 *	\details If the asset cannot be created, and empty QSharedPointer is returned and an error string explaining
 *	the problem is set into the error parameter.  The created child Asset is added to this DataStore and initialized
 *	to its default state.  childAddedAfterDeserialize() is emitted and this DataStore is setEdited().
 *	\sa AddChild(), setEdited(), initializeToDefault()
 */
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

/*! \brief Creates a child asset with the input XML details, adds it as a child and returns it.
 *	\details This functions just like createChildAsset() except that instead of being initialized
 *	to a default state, the child is initialized according to the passed in XML.  For more details, 
 *	see createChildAsset().
 *	\sa createChildAsset(), AddChild(), setEdited(), initializeToDefault()
 */
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

/*! \brief The idea here is to return this DataStore() to a pre-deserialized() state so that deserializeFromXml() can be
 *	be called again.  I don't think we ever used it, so don't put too much stock in it.
 */
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

/*! \brief A convenience function equivalent to getting the property with propName and calling its Property::setRuntimeEditable() function.
 *	See that function for more details.
 */
void DataStore::setPropertyRuntimeEditable(QString propName, bool editable)
{
	propertyContainer_->getProperty(propName)->setRuntimeEditable(editable);
}

/*! \brief Sets whether this DataStore and its Descendants should act as if they are in an experimental run or in a design environment.
	\details See DesignRoot::enableRunMode() for more detail.
 */
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

/*! \brief Returns all descendants that are runtime editable.
 *	\details See isRuntimeEditable() for more detail.
 */
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
/*! \brief Returns a list of all Property children of this DataStore and all of its descendants.
 *	\details Returned values are according to a Pre-Ordered Depth-First-Search
 *	\sa AddAssociateChild()
 */
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

/*! \brief Returns this DataStore's assetId.
 */
int DataStore::getAssetId()
{
	return assetId_;
}
/*! \brief Sets this DataStore's assetId and emits assetIdEdited() */
void DataStore::setAssetId(int id)
{
	assetId_ = id;
	emit assetIdEdited();
}

/*! copydoc Asset::upgradeVersion()
 *	\details In the case of the DataStore, when called, upgradeVersion is called on all of its
 *	descendants recursively as well.
 */
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

/*! \brief Returns true if this object is part of the requested search (ie. It falls into the searchRequest's group)
 *	\sa SearchRequest
 */
bool DataStore::isPartOfSearch(SearchRequest searchRequest)
{
	if(searchRequest.group & SearchRequest::EXPERIMENT)
		return true;
	return false;
}

/*! \brief Returns true if the input SearchRequest applies to this DataStore and is found in one of its searchable properties.
 */
bool DataStore::searchForQuery(SearchRequest searchRequest)
{
	if(!isPartOfSearch(searchRequest))
		return false;
	return executeSearchAlgorithm(searchRequest);
}
/*! \brief Returns true if the input SearchRequest applies to this DataStore or one of its descendants and is found in one of it or its descendants' searchable properties.
 *	\note AssociateElement descendants are searched as well
 */
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
/*! \brief Returns true if the input SearchRequest applies one of this DataStore's children and is found in one their searchable properties.
 *	\note AssociateElement children are searched as well
 */
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
/*! \brief Returns true if the input SearchRequest applies to this DataStore's descendants and is found in one of their searchable properties.
 *	\note AssociateElement descendants are searched as well
 */
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

/*! \brief Returns true if the input SearchRequest applies to this DataStore's parent or one of its parent's children and is found in one of their searchable properties.
 *	\note AssociateElements are searched as well
 */
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
/*! \brief Returns true if the input SearchRequest applies to one of this DataStore's ancestors and is found in one of their searchable properties.
 */
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

/*! \brief This slot is called when a child is edited and causes this DataStore's edited() signal to be emitted as well.
 */
void DataStore::childEdited()
{
	emit edited();
}

}; //namespace Picto
