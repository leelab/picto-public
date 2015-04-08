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
	//Since receiveEditedSignal and receiveDeleteSignal are the places where the edited_ and deleted_ flags are actually
	//  set, we connect our edited() and deleted() signals to them.
	connect(this, SIGNAL(edited()), this, SLOT(receivedEditedSignal()));
	connect(this, SIGNAL(deleted()), this, SLOT(receivedDeletedSignal()));
}
/*! \brief Converts this Asset into an XML based QString using the input QXmlStreamWriter.
 *	\details If the asset has been marked deleted, its returned XML code is empty.  Also, preSerialize() is called before
 *	the actual serialization begins and a boolean is returned indicating whether the Asset was successfully serialized.
 */
bool Asset::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(isDeleted())
		return true;
	preSerialize();
	return serializeAsXml(xmlStreamWriter);
}

/*! \brief Converts the XML code passed in inside a QXMLStreamReader into an Asset.
 *	\details Before deserialization starts, the preDeserialize() function is called. After serialization, edited_, isNew_
 *	and deleted_ flags are all set to false, and if deserialization was successful, postDeserialize() is called and true
 *	returned.  If deserialization was not successful, false is returned and postDeserialize is not called.
 */
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

/*! \brief Returns true if this asset and all of its children have valid syntax.
 *	\details It is awkward to call validateTree on an Asset which has no way to store children.  Probably, this function
 *	should be in the DataStore class and Asset should only expose validateObject().  It might be a good idea to change
 *	that at some point, but be very careful.  This all results from the fact Asset object's can all have parents but
 *	can't all have kids.
 */
bool Asset::validateTree()
{
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader());
	bool returnVal = validateObject(xmlStreamReader);
	return returnVal;
}

/*! \brief Initializes this asset into its default state
 *	\details Assets are not ready to be used when they are first created.  Much of their initialization occurs in the
 *	postDeserialize() function that is only called when serialization is complete.  They therefore need to have some XML
 *	deserialized even if it is just an empty tag in order to be sure that they are initialized and ready to be used.
 *	This function essentially creates an empty tag using the passed in tagName and type and deserializes this asset from
 *	that XML. When deserialization from the empty tag is complete, the Asset is marked asNew_ since no actual saved XML
 *	data had to be used in setting it up.  In some cases, the fact that the Asset is marked asNew_ will mean that it does
 *	not need to be serialized out.
 *	\sa DataStore::deserializeFromXml(), DataStore::serializeAsXml()
 */
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

/*! \brief Initializes the asset from the input XML without marking it as "not new" as would generally occurs as soon was
 *	deserialized from XML.  
 *	\details Essentially, this function just calls fromXml() and then sets isNew_ to true.  This way the serialization
 *	system can recognize that this asset was initialized to some value but wasn't actually serialized in from scratch.
 *	This is useful in some of the complex Serialization logic, particularly in DataStore.
 *	\sa DataStore::deserializeFromXml(), DataStore::serializeAsXml()
 */
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

/*! \brief Lets this asset know if the Asset is being used in a Design mode or an experimental execution mode.
 *	\sa DesignRoot::enableRunMode()
 */
void Asset::enableRunMode(bool)
{
	
}
/*! \brief Emits the deleted() signal when called to inform interested parties that this asset needs to be removed from
 *	the design.
 *	\details Since internally we connect deleted() to receivedDeletedSignal(), it also causes our deleted_ flag to be set
 *	so that this object itself will know if it has been deleted_.
 *	\note Currently in Picto, in order to delete an asset we just set it as deleted, then when the time comes for
 *	serialization, we don't serialize out any of the "delete marked" assets.  Doing things this way helps us maintain a
 *	general order and structure in xml design files from one serialization to the next to aid in comparison of different
 *	versions of a design file.
 *	\sa DataStore::serializeAsXml(), DataStore::deserializeFromXml()
 */
void Asset::setDeleted()
{
	emit deleted();
}
/*! \brief Sets this Asset's edited_ flag to true so that it will know that it has been changed since it was deserialized.
 */
void Asset::receivedEditedSignal()
{
	edited_ = true;
}
/*! \brief Sets this Asset's deleted_ flag to true so that it will know that it should not be serialized out.
 */
void Asset::receivedDeletedSignal()
{
	deleted_ = true;
}

/*! \brief Returns the "::" separated path to this Asset in the Asset tree.
 *	\details The path is returned up to the level of the experiment or AssociateRoot those levels are not included in the
 *	path.  
 *	\note Experiment=TestExp, Task=ATask, TopStateMachine=Top, Element=FirstState would have path: "ATask::Top::FirstState"
 */
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

/*! \brief Upgrades the version of this asset from the input version to the current version.
 *	\attention Classes that extend this function must always start the function by calling their parent class's
 *	implementation of upgradeVersion().
 */
void Asset::upgradeVersion(QString)
{
	
}
/*! \brief Sets the DesignConfig object managing the current Design to this Asset.
 *	\details Every Design has a single DesignConfig object that manages global issues affecting the entire design.  It can
 *	access every asset in the design and is accessible from every asset in the design.  It is set to an asset using the
 *	setDesignConfig function, typically by the Asset's parent.
 *	\note There is high potential here for a memory leak due to circular shared pointers.  To avoid this, Assets maintain
 *	SharedPointers to the DesignConfig but the DesignConfig uses WeakPointers to all assets.
 *	\sa getDesignConfig, DesignConfig
 */
void Asset::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
{
	designConfig_ = designConfig;
	designConfig_->addManagedAsset(selfPtr());
}

/*! \brief Sets the TaskConfig object managing the current Task to this Asset.
*	\details Every Design has one TaskConfig object for each task.  The TaskConfig object manages task-global issues.
*	It is accessible from every asset in the design.  It is set to an asset using the setTaskConfig function.
*	\note There is high potential here for a memory leak due to circular shared pointers.  To avoid this, Assets maintain
*	SharedPointers to the DesignConfig but the DesignConfig uses WeakPointers to all assets.
*	\sa getTaskConfig, TaskConfig
*/
void Asset::setTaskConfig(QSharedPointer<TaskConfig> taskConfig)
{
	taskConfig_ = taskConfig;
}

/*! \brief This function is called in fromXml() before deserialization and is used to prepare the Asset for
 *	deserialization.
 *	\details In the case of the Asset class, during preDeserialize we get the object's parent object's DesignConfig and
 *	set it to this Asset.
 *	\attention Any descendant classes extending this function should begin the function by calling their parent's
 *	implementation of preDeserialize().
 */
void Asset::preDeserialize()
{
	QSharedPointer<Asset> parent = getParentAsset();
	if(!parent)
		return;
	QSharedPointer<DesignConfig> designConfig = parent->getDesignConfig();
	if(!designConfig)
		return;
	setDesignConfig(designConfig);

	QSharedPointer<TaskConfig> taskConfig = parent->getTaskConfig();
	if (taskConfig)
	{
		setTaskConfig(taskConfig);
	}
	
}

/*! \brief This function is called in fromXml() after successful deserialization and is used to initialize the Asset based
 *	on values that were loaded in during deserialization.
 *	\details In the case of the Asset class, during postDeserialize we tell the DesignConfig that if we have reached the
 *	stage of loading where Asset IDs must be unique, it should make sure that our Asset ID is different from all others
 *	in this design.
 *	\sa DesignConfig::fixDuplicatedAssetIds()
 */
void Asset::postDeserialize()
{
	//This makes sure my id isn't duplicated.  It only actually does anything after someone called
	//  disallowDuplicateAssetIds() which doesn't happen until the end of experiment deserialization so that we're not
	//  crazily rewriting asset ids every time we deserialize.  This does, however, assure us that assets that are
	//  created after experiments are deserialized will be setup with nonduplicated ids.
	if (designConfig_)
	{
		designConfig_->fixDuplicatedAssetIds();
	}
}

/*! \brief A convenience function for adding an error to the list returned by Serializable::getErrors().
 *	\details This function takes the input error message, adds the path to this asset, formats a new 'pretty' error
 *	message accordingly, and calls Serializable::addErrorToList() with the formatted message.
 *	\sa Serializable::addErrorToList()
 */
void Asset::addError(QString errorMessage)
{
	QString newErr = "<b>Error:</b> <i>" + getPath() + "</i> - " + errorMessage;
	addErrorToList(newErr);
}

}; //namespace Picto
