#include <QVariant>

#include "DataStore.h"
#include "../property/Property.h"

namespace Picto {

QStringList DataStore::errors_;
qulonglong DataStore::lastDataID_ = 0;

DataStore::DataStore():
QObject(NULL),
dataID_(0),
isNew_(false),	//!!!!!!!!THIS NEEDS TO BE INITIALIZED TO TRUE.  I JUST CHANGED IT FOR DEBUGGING PURPOSES!!!!!!!!!!!!!
edited_(false),
deleted_(false)
{
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
		tagText_ = "<DEFAULTNAME/>";
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
	QMap<QString,QList<QSharedPointer<DataStore>>::iterator> readIterators;
	QMap<QString,QList<QSharedPointer<DataStore>>::iterator> readEndIterators;
	for(QMap<QString,QList<QSharedPointer<DataStore>>>::iterator iter=children_.begin();iter!=children_.end();iter++)
	{
		readIterators[iter.key()] = (*iter).begin();
		readEndIterators[iter.key()] = (*iter).end();
	}
	QMap<QString,QList<QSharedPointer<DataStore>>::iterator>::iterator dataIter, endIter;

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
		//Get the DataStore for the tag 
		Q_ASSERT_X(readIterators[name] != readEndIterators[name],"DataStore::serializeAsXml","The DataStore list ran out of entries while a deserialized DataStore of its type was not yet read from the original deserialized string.");
		QSharedPointer<DataStore> dataStore = *(readIterators[name]++);
		Q_ASSERT_X(!dataStore.isNull(),"DataStore::serializeAsXml","A DataStore pointer in this DataStore's list is null.");
		//Deserialize all new datastores until you get to one that isn't new
		while(dataStore->isNew())
		{
			dataStore->serializeAsXml(xmlStreamWriter);
			Q_ASSERT_X(readIterators[name] != readEndIterators[name],"DataStore::serializeAsXml","The DataStore list ran out of entries while a deserialized DataStore of its type was not yet read from the original deserialized string.");
			dataStore = *(readIterators[name]++);		
		}
		//This is the datastore represented by this tag.  Deserialize it.
		dataStore->serializeAsXml(xmlStreamWriter);
		//Move read pointer forward.  The next thing in it should be an end tag, and we don't want
		//to write that out because the child datastore already wrote out its end tag.
		xmlReader->readNext();
	}

	//Write out everything that wasn't yet written that needs to be
	dataIter = readIterators.begin();
	endIter = readEndIterators.begin();
	QSharedPointer<DataStore> childDataStore;
	while(dataIter!=readIterators.end())
	{
		while((*dataIter) != (*endIter))
		{
			childDataStore = (*(*dataIter));
			//If anything was deleted, it must have been in the original XML,
			//otherwise the system would have removed it from our list.
			Q_ASSERT(!childDataStore->isDeleted());
			//If the childDataStore wasEdited and it wasn't deserialized
			//in from the XML, it must have been a default value that was
			//altered by the user.  It must now be serialized out.  If
			//the dataStore isNew() it must have been added by the user since
			//we serialized in and must also be serialized out.
			//!!!!!!!!!!!!FOR TESTING NOW THIS IS JUST 1, FIX THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if(childDataStore->wasEdited() || childDataStore->isNew())
				childDataStore->serializeAsXml(xmlStreamWriter);
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
	foreach(QSharedPointer<DataStoreFactory> factory,factories_)
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
	//Loop until we're done with the tag or we reach the end of the XMLStream
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == myTagName_)) && !xmlStreamReader->atEnd())
	{
		//Move forward in XMLStream
		xmlStreamReader->readNext();
		xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets add the current tag to our tagText.
		//Continue loop if we're not at a start element
		if(!xmlStreamReader->isStartElement())
			continue;
		//Get Tag name
		QString name = xmlStreamReader->name().toString();
		//See if there's a datastore factory for this tag.
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
		//Get the factory for the tag 
		QSharedPointer<DataStoreFactory> factory = factories_.value(name);
		if(factory.isNull())
		{
			//If factory is null, this is a place holder tag to be ignored
			continue;
		}
		//Get the new DataStore
		//
		//The command below will put an empty string into type if the "type" attribute
		//isn't found.  This should map to the default value in the factory.
		QString type = xmlStreamReader->attributes().value("type").toString();
		QString error = "";
		QSharedPointer<DataStore> newChild = factory->getDataStore(error,type);
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
		returnVal &= newChild->deserializeFromXml(xmlStreamReader);
		xmlWriter->writeCurrentToken(*xmlStreamReader);	// Lets add the closing tag from the child to our tagText.
		//Add the new child datastore to the children map, and continue 
		//the loop
		children_[name].push_back(newChild);
	}
	//Make sure we got to the end tag
	if(xmlStreamReader->atEnd())
	{
		returnVal = false;
		addError(myTagName_.toAscii(), "Unexpected end of document", xmlStreamReader);
	}
	//Generate any default datastores that were not in the XML
	for(QMap<QString,QSharedPointer<DataStoreFactory>>::iterator iter = factories_.begin();iter!=factories_.end();iter++)
	{
		if(iter.value().isNull())
			continue;	//Its a placeholder tag
		QSharedPointer<DataStore> newChild;
		forever{
			newChild = iter.value()->getRequiredDataStore();
			if(newChild.isNull())
				break;
			//Generate new XMLStreamReader that is an empty tag to deserialize the child into a default state.
			QString childTag = QString("<%1/>").arg(iter.key());
			QSharedPointer<QXmlStreamReader> emptyTagXML(new QXmlStreamReader(childTag));
			// Move read pointer to tag.
			while(!emptyTagXML->isStartElement()) 
				emptyTagXML->readNext();
			newChild->deserializeFromXml(emptyTagXML);
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
	edited_ = false;
	isNew_ = false;
	deleted_ = false;
	return returnVal;
}

void DataStore::setDeleted()
{
	deleted_ = true;
	foreach(QList<QSharedPointer<DataStore>> dataStoreList,children_)
	{
		foreach(QSharedPointer<DataStore> dataStore,dataStoreList)
		{
			dataStore->setDeleted();
		}
	}
}

void DataStore::AddDataStoreFactory(QString name,QSharedPointer<DataStoreFactory> factory)
{
	factories_[name]=factory;
}

void DataStore::AddProperty(QSharedPointer<DataStore> prop)
{
	AddDataStoreFactory(prop.staticCast<Property>()->name(),QSharedPointer<DataStoreFactory>(new DataStoreFactory(prop)));
}

QList<QSharedPointer<DataStore>> DataStore::getGeneratedDataStores(QString factoryName)
{
	if(!children_.contains(factoryName))
		return QList<QSharedPointer<DataStore>>();
	return children_[factoryName];
}

void DataStore::AddError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader, QSharedPointer<QStringList> errors)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += QString("Line: %1\n").arg(xmlStreamReader->lineNumber());
	newErr += "Name: " + xmlStreamReader->name().toString() + "\n";
	newErr += "Element: " + xmlStreamReader->tokenString() + "\n";
	newErr += "Message: " + errorMsg + "\n";
	errors->push_back(newErr);
}

void DataStore::childEdited()
{
	edited_ = true;
	emit edited();
}
//------------------------------------------------------------------------------------







/*! \brief Used to serialize a unique Data ID into this DataStore's XML record
 *	This may or may not be called by the child classes serializeAsXml function
 *	depending whether a unique dataID is desired for this DataStore
 */
bool DataStore::serializeDataID(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(dataID_ == 0)
		dataID_ = generateDataID();
	xmlStreamWriter->writeTextElement("dataid",QString::number(dataID_));
	return true;
} 

/*! \brief Deserializes the unique data ID from a DataStore's XML record if it exists.
 */
bool DataStore::deserializeDataID(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "dataid")
	{
		addError("DataStore", "Unexpected tag", xmlStreamReader);
		return false;
	}
	dataID_ = xmlStreamReader->readElementText().toLongLong();
	return true;
}

//! \brief Returns this datastore's dataID
qulonglong DataStore::getDataID()
{
	return dataID_;
}

/*! \brief Adds an error message to the list of errors
 *
 *	We maintain a list of errors so that errors can be tracked through the deserialization
 *	process.
 */
void DataStore::addError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += QString("Line: %1\n").arg(xmlStreamReader->lineNumber());
	newErr += "Name: " + xmlStreamReader->name().toString() + "\n";
	newErr += "Element: " + xmlStreamReader->tokenString() + "\n";
	newErr += "Message: " + errorMsg + "\n";

	errors_.append(newErr);
}

void DataStore::addError(QString objectType, QString errorMsg)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += "Message: " + errorMsg + "\n";

	errors_.append(newErr);
}


/*!	\brief Returns a string listing all errors that have occured
 */
QString DataStore::getErrors()
{
	QString errorString = "DESERIALIZATION ERRORS\n\n";

	for(int i=0; i<errors_.length(); i++)
	{
		errorString += errors_[i] + "\n";
	}
	return errorString;

}

qulonglong DataStore::generateDataID()
{
	return ++lastDataID_;
}

void DataStore::serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
						QString name, QPoint point)
{
	Q_ASSERT(!name.isEmpty());
	xmlStreamWriter->writeStartElement(name);
	xmlStreamWriter->writeAttribute("datatype", "QPoint");
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(point.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(point.y()));
	xmlStreamWriter->writeEndElement();

}

void DataStore::serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
						QString name, QRect rect)
{
	Q_ASSERT(!name.isEmpty());
	xmlStreamWriter->writeStartElement(name);
	xmlStreamWriter->writeAttribute("datatype", "QRect");
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(rect.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(rect.y()));
	xmlStreamWriter->writeAttribute("width",QString("%1").arg(rect.width()));
	xmlStreamWriter->writeAttribute("height",QString("%1").arg(rect.height()));
	xmlStreamWriter->writeEndElement();

}

void DataStore::serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QColor color)
{
	Q_ASSERT(!name.isEmpty());
	xmlStreamWriter->writeStartElement(name);
	xmlStreamWriter->writeAttribute("datatype", "QColor");
	xmlStreamWriter->writeAttribute("R",QString("%1").arg(color.red()));
	xmlStreamWriter->writeAttribute("G",QString("%1").arg(color.green()));
	xmlStreamWriter->writeAttribute("B",QString("%1").arg(color.blue()));
	xmlStreamWriter->writeAttribute("A",QString("%1").arg(color.alpha()));
	xmlStreamWriter->writeEndElement();

}


QPoint DataStore::deserializeQPoint(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QPoint point;
	point.setX(xmlStreamReader->attributes().value("x").toString().toInt());
	point.setY(xmlStreamReader->attributes().value("y").toString().toInt());
	return point;
}

QRect DataStore::deserializeQRect(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QRect rect;
	rect.setX(xmlStreamReader->attributes().value("x").toString().toInt());
	rect.setY(xmlStreamReader->attributes().value("y").toString().toInt());
	rect.setWidth(xmlStreamReader->attributes().value("width").toString().toInt());
	rect.setHeight(xmlStreamReader->attributes().value("height").toString().toInt());
	return rect;
}

QColor DataStore::deserializeQColor(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QColor color;
	color.setRed(xmlStreamReader->attributes().value("R").toString().toInt());
	color.setGreen(xmlStreamReader->attributes().value("G").toString().toInt());
	color.setBlue(xmlStreamReader->attributes().value("B").toString().toInt());
	color.setAlpha(xmlStreamReader->attributes().value("A").toString().toInt());
	return color;
}

}; //namespace Picto
