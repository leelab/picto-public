#include "Asset.h"
#include "../memleakdetect.h"
namespace Picto {

Asset::Asset():
Serializable(),
QObject(NULL),
isNew_(true),
edited_(false),
deleted_(false),
assetId_(0),
hasAssetId_(false),
needsUniqueName_(false)
{
	connect(this, SIGNAL(edited()), this, SLOT(receivedEditedSignal()));
	connect(this, SIGNAL(deleted()), this, SLOT(receivedDeletedSignal()));
}

bool Asset::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(isDeleted())
		return true;
	preSerialize();
	return serializeAsXml(xmlStreamWriter);
}

bool Asset::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	preDeserialize();
	bool returnVal = deserializeFromXml(xmlStreamReader);
	edited_ = false;
	isNew_ = false;
	deleted_ = false;
	if(returnVal)
		postDeserialize();
	return returnVal;
}

bool Asset::validateTree()
{
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader());
	bool returnVal = validateObject(xmlStreamReader);
	return returnVal;
}

void Asset::initializeToDefault(QString tagName, QString type)
{
	QString tag = tagName;
	if(tag == "")
		tag = identifier();
	Q_ASSERT(tag != "");
	QString emptyTag = QString("<%1").arg(tag);
	if(type != "")
		emptyTag.append(QString(" type=\"%1\"").arg(type));
	emptyTag.append("/>");
	initializeFromXml(emptyTag);
	//Q_ASSERT_X(success,"Asset::initializeToDefault","Failed to initialize Asset object to default values.\nError:\n"+Serializable::getErrors().toLatin1());
}

//This function initializes the asset from XML without marking it as (not new) as generally
//occurs as soon as something is deserialized from XML.  This way the serialization system
//will recognize that this asset needs to be serialized out, because generally if an asset
//is not new and edited, if its tag is not found in the original serial text, the implecation
//is that it was actually there but just had its name changed automatically so it shouldn't be
//serialized out again... yeah... we need to rework the serialization system at some point.
bool Asset::initializeFromXml(QString xml)
{
	QSharedPointer<QXmlStreamReader> xmlStreamReader = QSharedPointer<QXmlStreamReader>(new QXmlStreamReader(xml));
	while(!xmlStreamReader->isStartElement() && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
	}
	if(xmlStreamReader->atEnd())
		return false;
	bool success = fromXml(xmlStreamReader);
	isNew_ = true;
	return success;
}

void Asset::enableRunMode(bool)
{
	
}

void Asset::setDeleted()
{
	emit deleted();
}

void Asset::receivedEditedSignal()
{
	edited_ = true;
}
void Asset::receivedDeletedSignal()
{
	deleted_ = true;
}

/*!	\brief Reinitializes this asset from its child assets.
 *
 *	This just calls postDeserialize(), which is the function that is called after
 *	serialization to allow the object to set itself up based on its recently added
 *	children.
 */
void Asset::reinitialize()
{
	//Ideally postDeserialize calls will be callable over and over again, but in practice, this isn't yet the case.
	//postDeserialize();
}

QString Asset::getPath()
{
	if(inherits("Picto::Experiment") || inherits("Picto::AssociateRoot"))
		return "";
	QString returnVal = getName();
	QSharedPointer<Asset> curr = getParentAsset();
	while(curr)
	{
		if(curr->inherits("Picto::Experiment") || inherits("Picto::AssociateRoot"))
			break;
		returnVal.prepend(QString("%1::").arg(curr->getName()));
		curr = curr->getParentAsset();
	}
	return returnVal;
}

//Should be overloaded to upgrade the version of this asset from the input version to the current version.
//Make sure to always start the function by calling the parent version of upgradeVersion()
void Asset::upgradeVersion(QString)
{
	
}

void Asset::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
{
	designConfig_ = designConfig;
	designConfig_->addManagedAsset(selfPtr());
}

void Asset::preDeserialize()
{
	QSharedPointer<Asset> parent = getParentAsset();
	if(!parent)
		return;
	QSharedPointer<DesignConfig> designConfig = parent->getDesignConfig();
	if(!designConfig)
		return;
	setDesignConfig(designConfig);
}

void Asset::postDeserialize()
{
	//This makes sure my id isn't duplicated.  It only
	//actually does anything after someone called disallowDuplicateAssetIds()
	//which doesn't happen until the end of experiment deserialization so that
	//we're not crazily rewriting asset ids every time we deserialize.  This
	//does, however, assure us that assets that are created after experiments
	//are deserialized will be setup with nonduplicated ids.
	if(designConfig_)
		designConfig_->fixDuplicatedAssetIds();
}

void Asset::addError(QString errorMessage)
{
	QString newErr = "<b>Error:</b> <i>" + getPath() + "</i> - " + errorMessage;
	addErrorToList(newErr);
}

}; //namespace Picto
