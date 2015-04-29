#include "AssociateRootHost.h"
#include "AssociateRoot.h"
#include "../design/PictoData.h"
#include "AssociateElement.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new AssociateRoot.
 *	\details Adds "AssociateId", "LinkedHostId" and "LinkedHostName" Properties.
 */
AssociateRoot::AssociateRoot()
{
	AddDefinableProperty(QVariant::Uuid,"AssociateId",QVariant());
	AddDefinableProperty(QVariant::Uuid,"LinkedHostId",QVariant());
	AddDefinableProperty(QVariant::String,"LinkedHostName",QVariant());

	requireUniqueName(false);
}

/*! \brief Links this AssociateRoot to the input AssociateRootHost.
 *	\details Calls setLinkedAsset() to go through the details of linking this AssociateRoot to the input
 *	AssociateRootHost().  Also goes through all of this AssociateRoot's children and attempts to link them to their
 *	corresponding Assets in the AssociateRootHost's tree using saved "link asset ID" and "link asset path" data.
 *	Returns true on success, false if the linkage wasn't perfect.  On false, the feedback input will be set to a message
 *	with more information.
 *	\note This could use some debugging.  Make sure, for example, that adding an AnalysisScriptHolder to a NumberVariable
 *	either won't work or at least won't break anything.  Possibly fix these types of issues by overloading the
 *	AssociateElement's attachToLinkedAsset function for different types of classes depending on whether they can be loaded
 *	onto the wrong element.
 *	\note This function would be better named linkToHost() or something like that. It is called LinkToAsset() for
 *	historic reasons.
 */
bool AssociateRoot::LinkToAsset(QSharedPointer<Asset> asset, QString& feedback, bool tempAsset)
{
	if(asset == linkedAsset_.toStrongRef())
		return true;

	feedback = "";
	AssociateElement* associateElement;
	bool linkedAssetMatches = isLinkableAsset(asset);
	QStringList pathMatchedAssociateRootAssets;
	QStringList unmatchedAssociateRootAssets;
	//Set this asset as our linked asset
	setLinkedAsset(asset, tempAsset);
	foreach(QString childTag,getValidChildTags())
	{
		foreach(QSharedPointer<Asset> child,getGeneratedChildren(childTag))
		{
			associateElement = dynamic_cast<AssociateElement*>(child.data());
			if(!associateElement || child->isDeleted())
				continue;

			//Linked Asset needs to be stored before we call this, since associateElement gets it from us.
			int result = associateElement->attachToLinkedAsset(linkedAssetMatches);
			switch(result)
			{
			case AssociateElement::SUCCESS_BY_LINK:
				break;
			case AssociateElement::SUCCESS_BY_PATH:
				pathMatchedAssociateRootAssets.append(associateElement->getInfoString());
				break;
			case AssociateElement::FAIL:
				unmatchedAssociateRootAssets.append(associateElement->getInfoString());
				child->setDeleted();
				break;
			}
			associateElement->postLinkUpdate();
		}
	}
	if(pathMatchedAssociateRootAssets.size() || unmatchedAssociateRootAssets.size())
	{
		QString linkedAssetType = asset->identifier();
		if(linkedAssetMatches)
			feedback = QString("The %1 or %2 design appears to have been altered outside of the Picto Development Environment.  ").arg(linkedAssetType).arg(friendlyTypeName());
		else
			feedback = QString("This %1 was built for a %2 design that does not appear in this experiment.  ").arg(friendlyTypeName()).arg(linkedAssetType);
		if(unmatchedAssociateRootAssets.isEmpty())
		{
			feedback += QString("All elements of the %1 were succesfully attached to this experiment's %2 %3.").arg(friendlyTypeName()).arg(asset->getName()).arg(linkedAssetType);
		}
		else
		{
			feedback += QString("Experimental element links could not be found for the following %1 elements which have been removed:\n").arg(friendlyTypeName()) 
				+ unmatchedAssociateRootAssets.join("\n") + "\n";
		}
		if(linkedAssetMatches && pathMatchedAssociateRootAssets.size())
		{
			feedback += QString("Imperfect matches were detected for the following %1 elements:\n").arg(friendlyTypeName()) + unmatchedAssociateRootAssets.join("\n") + "\n";
		}

	}

	if(feedback.size())
		return false;
	return true;
}

/*! \brief Searches this object's parent's (PictoData) tree for an AssociateRootHost with the linked host ID set for this
 *	AssociateRoot and returns it as an Asset.  Returns an empty pointer otherwise.
 */
QSharedPointer<Asset> AssociateRoot::getLinkableAsset()
{
	QSharedPointer<PictoData> parent = getParentAsset().staticCast<PictoData>();
	foreach(QString childTag,parent->getValidChildTags())
	{
		foreach(QSharedPointer<Asset> child,parent->getGeneratedChildren(childTag))
		{
			if(isLinkableAsset(child))
				return child;
		}
	}
	return QSharedPointer<Asset>();
}

void AssociateRoot::postDeserialize()
{
	UIEnabled::postDeserialize();
	if(getAssociateId() == QUuid())
		propertyContainer_->setPropertyValue("AssociateId",QUuid::createUuid());
	setDescendantPropertiesAsAssociates();
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(AssociateRootChildWasAdded(QSharedPointer<Asset>)));
}

bool AssociateRoot::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Performs operations necessary to link this object with the input AssociateRootHost object.
 *	\details Informs the input AssociateRootHost that this AssociateRoot is linking to it.
 *	Connects the various signals and slots that need to be connnected.  Updates this objects Properties to store the
 *	connection to the AssociateRootHost by calling updateLinkedAssetProperties().  Sets the LinkedHostId and
 *	LinkedHostName properties to the input AssociateRootHost's information.  
 */ 
void AssociateRoot::setLinkedAsset(QSharedPointer<Asset> asset, bool tempAsset)
{
	Q_ASSERT(asset);
	if(asset == linkedAsset_.toStrongRef())
		return;
	//If this AssociateRoot was already linked to an asset, disconnect that asset's Id and Name property signals
	if(!linkedAsset_.isNull())
	{
		QSharedPointer<Property> linkedIdProp = getLinkedAssetHostIdProperty();
		Q_ASSERT(linkedIdProp);
		disconnect(linkedIdProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(linkedAssetPropertyEdited(Property*,QVariant)));
		QSharedPointer<Property> linkedNameProp = getLinkedHostNameProperty();
		if(linkedNameProp)
			disconnect(linkedNameProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(linkedAssetPropertyEdited(Property*,QVariant)));		
	}
	linkedAsset_ = asset;
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);

	//We skip overwriting the AssociateRoot with our temporary analysis.
	if (!tempAsset)
	{
		assocRootHost->setAssociateRoot(identifier(), selfPtr().staticCast<AssociateRoot>());
	}
	updateLinkedAssetProperties();

	//Connect to change signals from linked assets's Id and Name properties, so that we can update when
	//they change
	QSharedPointer<Property> linkedIdProp = getLinkedAssetHostIdProperty();
	Q_ASSERT(linkedIdProp);
	connect(linkedIdProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(linkedAssetPropertyEdited(Property*,QVariant)));
	QSharedPointer<Property> linkedNameProp = getLinkedHostNameProperty();
	if(linkedNameProp)
		connect(linkedNameProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(linkedAssetPropertyEdited(Property*,QVariant)));
}

/*!	\brief Updates this objects Properties to store the connection to the AssociateRootHost by calling
 *	updateLinkedAssetProperties().
 *	\details Sets the LinkedHostId and LinkedHostName properties to the linked AssociateRootHost's information.  
 */
void AssociateRoot::updateLinkedAssetProperties()
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);
	propertyContainer_->setPropertyValue("LinkedHostId",assocRootHost->getHostId());
	propertyContainer_->setPropertyValue("LinkedHostName",linkedAsset_.toStrongRef()->getName());
}

/*! \brief Returns true if the input asset is a AssociateRootHost with host ID equal to this AssociateRoot's saved
 *	LinkedHostId.
 */
bool AssociateRoot::isLinkableAsset(QSharedPointer<Asset> asset)
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(asset.data());
	if(!assocRootHost)
		return false;
	QUuid hostId = assocRootHost->getHostId();
	QUuid linkedId = getLinkedHostId();
	QString hostIdStr = assocRootHost->getHostId().toString();
	QString linkedIdStr = getLinkedHostId().toString();
	if(assocRootHost->getHostId() == getLinkedHostId())
		return true;
	return false;
}

/*! \brief Returns a pointer to the linked AssociateRootHost's name Property.
 */
QSharedPointer<Property> AssociateRoot::getLinkedHostNameProperty()
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);
	return assocRootHost->getNameProperty();
}

/*! \brief Returns a pointer to the linked AssociateRootHost's Host Id Property.
 */
QSharedPointer<Property> AssociateRoot::getLinkedAssetHostIdProperty()
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);
	return assocRootHost->getHostIdProperty();
}

/*! \brief Called when one of the linked AssociateRootHost's Properties is edited.
 *	Calls updateLinkedAssetProperties() to update this object's corresponding Property values.
 */
void AssociateRoot::linkedAssetPropertyEdited(Property*,QVariant)
{
	updateLinkedAssetProperties();
}

/*! \brief Called whenever a child is added to this AssociateRoot.
 *	Sets all of its descendant properties to Associate properties.
 *	\sa DataStore::setDescendantPropertiesAsAssociates()
 */
void AssociateRoot::AssociateRootChildWasAdded(QSharedPointer<Asset> child)
{
	if(!child->inherits("Picto::DataStore"))
		return;
	child.staticCast<DataStore>()->setDescendantPropertiesAsAssociates();
}

}; //namespace Picto
