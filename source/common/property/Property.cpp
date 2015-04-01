#include <QScriptValue>
#include <QObject>
#include <QMetaType>

#include "Property.h"
#include "../memleakdetect.h"

namespace Picto {

bool Property::hadObsoleteSerialSyntax_ = false;
bool Property::valueWasChanged_ = true;
bool Property::expValueWasChanged_ = true;
QString Property::changedValueName_ = "";
QString Property::changedExpValueName_ = "";

/*! \brief Constructs a new Property object.
 *	\details The input type is the type of the QVariant value underlying this Property (ie. A QVariant::type).
 *	The input name is the name of this Property (ie. "Radius").  The input value is a default value for the
 *	Property which is set as the default saveValue, initValue and runValue for the Property.
 */
Property::Property(int type, QString name, QVariant value) :
saveValue_(value),
initValue_(value),
runValue_(value),
type_(type),
name_(name),
tagName_(""),
typeVal_(""),
scriptEditable_(true),
runtimeEnabled_(false),
index_(-1),
assetId_(0),
visible_(true),
guiEditable_(true),
serialSyntaxUpgraded_(false),
syncInitAndRunVals_(false),
runMode_(false),
associateProperty_(false)
{
	//Add the ID serialization attribute so that we can read in this property's ID.
	AddSerializationAttribute("id");
	requireUniqueName();
}

Property::~Property()
{
}

/*! \brief Returns the type of this Property.
 *	\details This is also the type of the QVariant underlying the Property defined in terms of QVariant::type.
 */
int Property::type()
{
	return type_;
}

/*! \brief Returns the name of this Property.
 *	\details This is the name that appears next to widgets with this Property's value (ie. "Radius).
 */
QString Property::getName()
{
	return name_;
}

/*! \brief Sets this Property's name.
 *	\sa getName()
 */
void Property::setName(QString name)
{
	name_ = name;
}

/*! \brief Returns this Property's initValue.
 *	\details A Property's initValue is an initial condition for the Property.  The Property's runValue, which is the one
 *	that actually takes effect during an experiment, is set to the Property's initValue every time the Property's parent
 *	enters scope.  This value is used during Experimental runs for Experiment Operator's to be able to change experimental
 *	parameters during the run.  Since the changes that the Operator makes only take effect when the Property's parent first
 *	enters scope, the Operator need not worry that depending on the current run state some invalid logic condition will be
 *	created changing a Property value.
 */
QVariant Property::initValue()
{
	return initValuePrivate();
}

/*! \brief Returns the Property's current value as a function of the current run mode.
 *	\details When run mode is enabled, the returned value is the runValue, the effective value of the Property throughout
 *	an experimental run.  If run mode is disabled, the returned value is the saveValue.  This is the value that is actually
 *	saved to disk when a Picto Design is saved.  It is important to differentiate between saveValue and runValue so that
 *	the same Design can be used in the StateMachineEditor and in the TestViewer.  If we only had a runValue, then tests
 *	run in the Picto TestViewer would affect the Design file that is saved to disk.
 *	By separating these two types of values, we can be sure that testing a design won't affect its saved file.
 *	\note We can't use initValues as saveValues because then changes that a test Operator makes to Property initValues
 *	would also affect the saved file.  By seperating out the saveValue though, since every time a Design is deserialized
 *	from xml the saveValues are the ones that are set to the initValue and runValue fields, we are sure that the initial
 *	conditions of a Design before it is run will always be according to the design's saveValues.  
 */
QVariant Property::value()
{
	if(runMode_)
	{
		return runValuePrivate();
	}
	else
		return saveValuePrivate();
}

/*! \brief Enables/Disables runMode for this Property.
 *	\details See value() for more detail on what this means.
 */
void Property::enableRunMode(bool enable)
{
	Asset::enableRunMode(enable);
	runMode_ = enable;
}

/*! \brief When InitRunValueSync is enabled, changing a Property's initValue immediately updates its runValue as well,
 *	with no need to wait for the Property parent to re-enter scope.
 *	\details This is useful for Properties like the Position signal calibration values.  They need to take effect
 *	right away.  We can't wait for some parent object to come back into scope.
 *
 *	By default, InitRunValueSync is disabled.
 */
void Property::enableInitRunValueSync(bool enable)
{
	syncInitAndRunVals_ = enable;
}

/*! \brief Sets the input _attributeName to the input _attributeValue.
 *	\details Some Property types require attributes.  The EnumProperty for example needs a list of possible enum values so
 *	that it can translate between an enum value name and an associate integer.  This function is used to set up that
 *	type of mapping.
 */
void Property::setAttribute(QString _attributeName, QVariant _attributeValue)
{
	attributes_[_attributeName] = _attributeValue;
}

/*! \brief Returns the value of the attribute with the input _attributeName.
 *	\details If no such _attributeName exists, an empty QVariant is returned.
 *	For more details on Property attibutes, see setAttribute()
 */
QVariant Property::attributeValue(QString _attributeName) const
{
	if(attributes_.contains(_attributeName))
		return attributes_.value(_attributeName);
	return QVariant();
}

/*! \brief Implements Asset::serializeAsXml() to serializes this Property into the input QXmlStreamWriter.
 *	\details A Property is saved to XML as follows
 *	\code <[TagName] name='someName' type='someType' id=[some number]>[some value]</[TagName]> \endcode
 *	All Properties use the "name" field.
 *	The "type" field may or may not be used.
 *	The "id" field is used for every Property with an AssetId, which should actually be every Property that there is.
 *	The id field is saved out as a serializationAttribute set in the setSerializationAttributeValue() function though.
 *	In theory, every attribute set with this function would be serialized out, but in practice we are only using
 *	setSerializationAttributeValue() for the serialization id.  It might be worth while cleaning this up at some point.
 *
 *	The actual Property value is stored between the opening and closing xml tags according to its string value as returned
 *	from variantToString().
 *	\sa deserializeFromXml()
 */
bool Property::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(!isNew() && !wasEdited() && !serialSyntaxUpgraded_)
	{
		QXmlStreamReader copyReader(tagText_);
		//Move to start element
		while(!copyReader.isStartElement())
			copyReader.readNext();
		while(!copyReader.atEnd())
		{
			xmlStreamWriter->writeCurrentToken(copyReader);
			copyReader.readNext();
		}
		return true;
	}
	if(tagName_ == "")
		tagName_ = getName();
	xmlStreamWriter->writeStartElement(tagName_);
	if(getName() != tagName_)
		xmlStreamWriter->writeAttribute("name",getName());
	//In cases where a Asset Factory used a type attribute to choose between types, a type that we don't use but need to
	//  write out would be in the tag.
	if(typeVal_ != "")
		xmlStreamWriter->writeAttribute("type",typeVal_);
	//UpdateSerializationAttributesFromValue();
	for(QMap<QString,QVariant>::iterator iter = serializationAttributes_.begin();iter != serializationAttributes_.end();iter++)
	{
		xmlStreamWriter->writeAttribute(iter.key(),iter.value().toString());
	}
	QString valToSave = variantToString(saveValuePrivate());
	if(valToSave.length())
		xmlStreamWriter->writeCharacters(valToSave);
	xmlStreamWriter->writeEndElement();
	return true;
}
/*! \brief Implements Asset::deserializeFromXml() to deserialize this Property from the Xml stored in the input
*	QXmlStreamReader.
 *	\details This function reads in XML code as output from serializeAsXml() and uses it to initialize this Property.
 *
 *	\note There is some code here that deals with loading Properties from an obsolete XML format.  In the past
 *	some Property types stored their values in XML attributes instead of between the opening and closing tags.  To deal
 *	with these cases, we deserialize all unexpected attributes into an "unexpectedAttributeMap" and then input that map
 *	into the attributeMapToVariantValue() function which can be implemented in child classes to retrieve the value from 
 *	those attributes.  When the Property is saved again, it will automatically be upgraded to the current Picto syntax.
 */
bool Property::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	Q_ASSERT_X(!runMode_,"Property::deserializeFromXml","deserializeFromXml should not be called when property is in run mode.");
	serialSyntaxUpgraded_ = false;
	//Create XMLStreamWriter to store this DataStore's tag text in a string
	tagText_ = "";
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&tagText_));
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the start tag to our tagText.

	//Get Start tag name (depending on who the parent is, they may have given us different names.
	tagName_ = xmlStreamReader->name().toString();
	bool emptyTag = true;
	bool hadName = false;
	if(xmlStreamReader->attributes().hasAttribute("name"))
	{
		setName(xmlStreamReader->attributes().value("name").toString());
		hadName = true;
	}
	else
		setName(tagName_);

	//In cases where a Asset Factory used a type attribute to choose between types, a type that we don't use but need to
	//  write out would be in the tag.
	if(xmlStreamReader->attributes().hasAttribute("type"))
	{
		typeVal_ = xmlStreamReader->attributes().value("type").toString();
	}
	else
		typeVal_ = "";

	//QXmlStreamAttributes attributeList = xmlStreamReader->attributes();
	//foreach(QXmlStreamAttribute attribute,attributeList)
	//{
	//	if(serializationAttributes_.contains(attribute.name().toString()))
	//	{
	//		serializationAttributes_[attribute.name().toString()] = attribute.value().toString();
	//		emptyTag = false;
	//	}
	//	else if((attribute.name() != "name") && (attribute.name() != "type") )
	//	{
	//		QString allowedAttributes = "name\ntype";
	//		foreach(QString attrName,serializationAttributes_.keys())
	//		{
	//			allowedAttributes.append("\n").append(attrName);
	//		}
	//		addError(getName().toLatin1(), QString("Invalid attribute: \n%1\nThe following attributes are defined for this property:\n%2\n-----").arg(attribute.name().toString()).arg(allowedAttributes).toLatin1(), xmlStreamReader);
	//		return false;
	//	}
	//}
	//Get the tag's attributes.  Put the attributes that are not expected (ie. obsolete) in an unexpectedAttributeMap
	QMap<QString,QVariant> unexpectedAttributeMap;
	QXmlStreamAttributes attributeList = xmlStreamReader->attributes();
	foreach(QXmlStreamAttribute attribute,attributeList)
	{
		if((attribute.name() != "name") && (attribute.name() != "type") )
		{
			emptyTag = false;
			if(serializationAttributes_.contains(attribute.name().toString()))
			{
				serializationAttributes_[attribute.name().toString()] = attribute.value().toString();
				continue;
			}
			//If we got here, there are unexpected attributes, tell the system that we encountered obsolete syntax.
			hadObsoleteSerialSyntax_ = true;
			serialSyntaxUpgraded_ = true;
			emit edited();
			unexpectedAttributeMap[attribute.name().toString()] = attribute.value().toString();
		}
	}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
	{
		addError("XML syntax has been violated.");
		return false;
	}
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Write everything left to tagText.
	if(!xmlStreamReader->isCharacters() && !xmlStreamReader->isEndElement())
	{
		addError(QString("Unexpected value read.").toLatin1());
		return false;
	}
	QString value = xmlStreamReader->text().toString();

//	QString value = xmlStreamReader->readElementText();
	if(value != "")
		emptyTag = false;
	
	// If its an empty tag, we should just go with the default value.
	if(!emptyTag)
	{
		QString error = "";
		QVariant result = stringToVariant(value,error);
		if(!error.isEmpty())
		{	//Maybe this is an older version of a property that stored its data in attributes
			//Try to get a variant value from the attributes
			QString attributeError = "";
			result = attributeMapToVariantValue(unexpectedAttributeMap, attributeError);
			//If we couldn't get a value, report an error
			if(!attributeError.isEmpty())
				error = error + "\n" + attributeError;	//Add error details
			else
			{
				error = "";	//No more error
			}
		}
		if(error.size())
		{
			addError(error.toLatin1());
			return false;
		}
		
		//Check if our serialization syntax will be upgraded on the next save out.
		QString reserialized = variantToString(result);
		if(value != reserialized)
		{
			//If we got here, syntax will be changed on the next save, tell the system that we encountered obsolete syntax.
			hadObsoleteSerialSyntax_ = true;
			serialSyntaxUpgraded_ = true;
			emit edited();
		}
		setValue(result);
	}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == tagName_)) && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
		xmlWriter->writeCurrentToken(*xmlStreamReader);// Write everything left to tagText.
	}
	//Make sure we didn't finish the document.
	if(xmlStreamReader->atEnd())
	{
		addError("Unexpected end of document");
		return false;
	}
	return true;
}

/*! \brief This is the string that is sent over the network when an initValue is changed and its value needs to be
 *	sent to a running session.
 *	\sa initValFromUserString()
 */
QString Property::initValToUserString()
{
	return variantToString(initValue());
}

/*! \brief Updates the current initValue from a string that was output from initValToUserString().
 *	\sa initValToUserString()
 */
void Property::initValFromUserString(QString userString)
{
	QString error = "";
	setInitValue(stringToVariant(userString,error));
}

/*! \brief Converts the current variant runValue to a user readable string for serialization purposes.
 *	\sa valFromUserString()
 */
QString Property::valToUserString()
{
	return variantToString(value());
}

/*! \brief Updates the current variant runValue from a user readable string in the format serialized out from
 *	valToUserString().
 *	\sa valToUserString()
 */
void Property::valFromUserString(QString userString)
{
	QString error = "";
	setValue(stringToVariant(userString,error));
}

/*! \brief Returns this Property's AssetId.
 *	\details It is retreived from the Property's id attribute.
 */
int Property::getAssetId()
{
	return GetSerializationAttributeValue("id").toInt();
}

/*! \brief Sets this Property's AssetId.
 *	\details It is set into the Property's id attribute.
 *	\note This function causes assetIdEdited() to be emitted.  It also emits edited() if the Property's value has been
 *	previously edited.  If the Property's value has not been previously edited, it is a default Property value and
 *	changing an AssetId is not a change that should cause this Property to need to be serialized out.
 */
void Property::setAssetId(int id)
{
	SetSerializationAttributeValue("id",QVariant(id));
	//If the asset is new and was not edited, it doesn't need to
	//be serialized out because its a default property value, so 
	//we should not emit edited just because we are changing its
	//asset id.
	if(!isNew() || wasEdited())
	{
		emit edited();
	}
	emit assetIdEdited();
}

/*! \brief Sets the value of the initValue to the input _value.  If enableInitRunValueSync() was used to enable
 *	init->runValue syncing, the runValue is set as well.
 */
void Property::setInitValue(QVariant _value)
{
	Q_ASSERT_X(runMode_,"Property::setInitValue","Init values should only be set in run mode");
	setInitValuePrivate(_value);
	if(syncInitAndRunVals_)
	{
		setRunValuePrivate(_value);
	}
}

/*! \brief Sets the input value to the appropriate saveValue, initValue and/or runValue according to the current runMode
 *	and InitRunValueSync state.
 *	\details See value() and enableInitRunValueSync() for more details.
 */
void Property::setValue(QVariant _value)
{
	if(!runMode_)
	{
		setSaveValuePrivate(_value);
		setInitValuePrivate(_value);
		setRunValuePrivate(_value);
	}
	else
	{
		setRunValuePrivate(_value);
		if(syncInitAndRunVals_)
			setInitValuePrivate(_value);
	}
}

/*! \brief Converts the input QVariant value to a string value for various purposes throughout this Property.
 *	\details This function is meant to be overridden by child classes to convert the input value to a string
 *	according to the child class's own special criteria.  By default it simply calls the QVariant::toString() function and
 *	returns that.
 *	\sa stringToVariant()
 */
QString Property::variantToString(QVariant value) const
{
	return value.toString();
}

/*! \brief Converts the input QString value to a QVariant value for various purposes throughout this Property.
 *	\details This function is meant to be overridden by child classes to convert the input value to a QVariant
 *	according to the child class's own special criteria.  By default it simply creates a QVariant with its QString
 *	constructor and returns that QVariant.
 *	\sa variantToString()
 */
QVariant Property::stringToVariant(QString string, QString& error) const
{
	error = "";
	return QVariant(string);
}

/*! \brief This is used to upgrade obsolete syntax that stored Property value data inside an xml tag's attribute fields
 *	during serialization.  A map of those attributes is input, and the function should be overriden by child classes to
 *	return the appropriate QVariant on the basis of that map.
 *	\details The error input reference ins filled with an error message if there is some problem with converting the
 *	input attribute map to a QVariant.
 */
QVariant Property::attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const
{
	error = "This property type does not support value definition by attribute values.";
	return QVariant();
}

/*! \brief Returns the current saveValue.
*/
QVariant Property::saveValuePrivate()
{
	return saveValue_;
}

/*! \brief Sets the current saveValue to the input.  If this causes it to change, the edited() signal is emitted.
 */
void Property::setSaveValuePrivate(QVariant _value)
{
	if(saveValue_ != _value)
	{
		saveValue_ = _value;
		emit edited();
	}
}

/*! \brief Returns the current initValue.
 */
QVariant Property::initValuePrivate()
{
	return initValue_;
}

/*! \brief Sets the current initValue to the input.  If this causes it to change, the initValueChanged() signal is emitted.
 */
void Property::setInitValuePrivate(QVariant _value)
{
	if(initValue_ != _value)
	{
		initValue_ = _value;
		emit initValueChanged(this,initValue_);
	}
}

/*! \brief Returns the current runValue.
 */
QVariant Property::runValuePrivate()
{
	return runValue_;
}

/*! \brief Sets the current runValue to the input.  If this causes it to change, the valueChanged() signal is emitted.
 *	\details This function also informing some static vairables that this Property value was changed.  This is useful
 *	for checking if an experimental script that is meant to be read only attempted to change a Property value, or for
 *	checking if an Analysis Script attempted to change an Experimental Property value.  In both of these cases, if 
 *	this type of change takes place it can be used to trigger a runtime error.
 *	\sa startMonitoringForValueChange(), startMonitoringForExperimentalValueChange(), valueWasChanged()
 *	\sa experimentalValueWasChanged(), changedValueName(), changedExperimentalValueName()
 */
void Property::setRunValuePrivate(QVariant _value)
{
	if(runValue_ != _value)
	{
		runValue_ = _value;
		if(!valueWasChanged_)
		{
			valueWasChanged_ = true;
			changedValueName_ = getName();
		}
		if(!expValueWasChanged_ && !associateProperty_)
		{
			expValueWasChanged_ = true;
			changedExpValueName_ = getName();
		}
		emit valueChanged(this,runValue_);
	}
}

/*! \brief Adds a new Attribute value that will be serialized out in this Property's XML tag.
 *	\details name is the name of the new attribute.  Its value can be set with SetSerializationAttributeValue().
 *	\sa SetSerializationAttributeValue(), GetSerializationAttributeValue(), serializeAsXml()
 */
void Property::AddSerializationAttribute(QString name)
{
	SetSerializationAttributeValue(name,QVariant());
}

/*! \brief Sets the value of Attribute with the input name to the input _value.
 *	\sa AddSerializationAttribute(), GetSerializationAttributeValue(), serializeAsXml()
 */
void Property::SetSerializationAttributeValue(QString name, QVariant _value)
{
	serializationAttributes_[name] = _value;
}

/*! \brief Gets the value of Attribute with the input name.
 *	\sa AddSerializationAttribute(), SetSerializationAttributeValue(), serializeAsXml()
 */
QVariant Property::GetSerializationAttributeValue(QString name)
{
	if(serializationAttributes_.contains(name))
		return serializationAttributes_[name];
	return QVariant();
}

}; //namespace Picto
