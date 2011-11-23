#include "Asset.h"

namespace Picto {

Asset::Asset():
Serializable(),
QObject(NULL),
isNew_(true),	//!!!!!!!!THIS NEEDS TO BE INITIALIZED TO TRUE.  I JUST CHANGED IT FOR DEBUGGING PURPOSES!!!!!!!!!!!!!
edited_(false),
deleted_(false),
assetId_(9),
hasAssetId_(false)
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

bool Asset::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader, bool validate)
{
	preDeserialize();
	bool returnVal = deserializeFromXml(xmlStreamReader,validate);
	edited_ = false;
	isNew_ = false;
	deleted_ = false;
	if(returnVal)
		postDeserialize();
	if(validate)
	{	
		//The XML syntax was fine.  Lets make sure that the object is initialized and valid.
		bool myResult = validateObject(xmlStreamReader);
		returnVal &= myResult;
		if(!myResult)
			addError(identifier().toAscii(),QString("The %1 Object's Structural XML syntax was correct but there were errors in tag contents.").arg(identifier()).toAscii(),xmlStreamReader);
	}
	return returnVal;
}

bool Asset::validateTree()
{
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader());
	bool returnVal = validateObject(xmlStreamReader);
	if(!returnVal)
		addError(identifier().toAscii(),QString("The %1 Object's Structural XML syntax was correct but there were errors in tag contents.").arg(identifier()).toAscii(),xmlStreamReader);
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
	bool success = fromXml(emptyTag);
	isNew_ = true;
	//Q_ASSERT_X(success,"Asset::initializeToDefault","Failed to initialize Asset object to default values.\nError:\n"+Serializable::getErrors().toAscii());
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
	if(inherits("Picto::Experiment"))
		return "";
	QString returnVal = getName();
	QSharedPointer<Asset> curr = getParentAsset();
	while(curr)
	{
		if(curr->inherits("Picto::Experiment"))
			break;
		returnVal.prepend(QString("%1::").arg(curr->getName()));
		curr = curr->getParentAsset();
	}
	return returnVal;
}

void Asset::setExperimentConfig(QSharedPointer<ExperimentConfig> expConfig)
{
	expConfig_ = expConfig;
	expConfig_->addManagedAsset(selfPtr());
}

void Asset::preDeserialize()
{
	QSharedPointer<Asset> parent = getParentAsset();
	if(!parent)
		return;
	QSharedPointer<ExperimentConfig> expConfig = parent->getExperimentConfig();
	if(!expConfig)
		return;
	setExperimentConfig(expConfig);
}

void Asset::postDeserialize()
{
	//This makes sure my id isn't duplicated.  It only
	//actually does anything after someone called disallowDuplicateAssetIds()
	//which doesn't happen until the end of experiment deserialization so that
	//we're not crazily rewriting asset ids every time we deserialize.  This
	//does, however, assure us that assets that are created after experiments
	//are deserialized will be setup with nonduplicated ids.
	if(expConfig_)
		expConfig_->fixDuplicatedAssetIds();
}
}; //namespace Picto
