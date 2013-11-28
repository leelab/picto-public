#include "AssociateHostLink.h"
#include "AssociateRoot.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an AssociateHostLink object
 *	\details Creates two Property objects.
 *	- ParentPath	- Holds a string with the path of the DataStore to which this object's owner will be an Associate child.
 *	- ParentId		- Holds the AssetId of the DataStore to which this object's owner will be an Associate child.
 */
AssociateHostLink::AssociateHostLink()
: DataStore()
{
	AddDefinableProperty(QVariant::String,"ParentPath",QVariant());
	AddDefinableProperty(QVariant::Int,"ParentId",0);
}

/*! \brief Creates a new AssociateHostLink and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> AssociateHostLink::Create()
{
	QSharedPointer<AssociateHostLink> newAssociateHostLink(new AssociateHostLink());
	newAssociateHostLink->setSelfPtr(newAssociateHostLink);
	return newAssociateHostLink;
}

/*! \brief Sets up this object to link to the input Asset.
 *	Stores a pointer to the input Asset.  Connects to the input Asset's Asset::assetIdEdited() and UIEnabled::nameEdited() signals so that this object
 *	will know when it has to change the values stored in its Properties, then calls updateLinkedAssetProperties() to
 *	update those Property values for this new Asset and emits the linkedToAsset() signal to tell the world that the linked Asset changed.
 */
void AssociateHostLink::linkToAsset(QSharedPointer<Asset> asset)
{
	Q_ASSERT(asset);
	if(linkedAsset_.toStrongRef() == asset)
		return;
	//If this link was already connected to an asset, disconnect its name and id changed signals
	if(!linkedAsset_.isNull())
	{
		connect(linkedAsset_.data(),SIGNAL(assetIdEdited()),this,SLOT(updateLinkedAssetProperties()));
		connect(linkedAsset_.toStrongRef().staticCast<UIEnabled>().data(),SIGNAL(nameEdited()),this,SLOT(updateLinkedAssetProperties()));		
	}
	linkedAsset_ = asset;
	//Connect to the assets name and id changed signals so that this link will be updated if they change.
	connect(linkedAsset_.data(),SIGNAL(assetIdEdited()),this,SLOT(updateLinkedAssetProperties()));
	connect(linkedAsset_.toStrongRef().staticCast<UIEnabled>().data(),SIGNAL(nameEdited()),this,SLOT(updateLinkedAssetProperties()));		
	updateLinkedAssetProperties();
	emit linkedToAsset(asset);
}

/*! \brief Updates the current linked path value by searching for the input oldPrefix string at the beginning of the path and replacing
 *	it with the newPrefix string.
 *	\details This is useful when we are exporting and importing multiple AssociateElements from one element into another.  In those cases,
 *	we want to maintain relative link paths for the different AssociateElements but change the overall location of the group.  This is equivalent
 *	to updating the beginning of all of their paths in the same way.
 */
void AssociateHostLink::updateLinkPath(QString oldPrefix,QString newPrefix)
{
	//No one should be trying to update this objects link path if it's already linked to an asset
	Q_ASSERT(linkedAsset_.isNull());

	//Replace the input oldPrefix in the link path with the input newPrefix.
	QString currentLinkPath = propertyContainer_->getPropertyValue("ParentPath").toString();
	int replacementRegionLength = oldPrefix.length();
	Q_ASSERT(currentLinkPath.left(replacementRegionLength) == oldPrefix);
	if(replacementRegionLength == 0 && newPrefix.length())
		newPrefix += "::";
	QString newLinkPath = currentLinkPath.replace(0,replacementRegionLength,newPrefix);
	propertyContainer_->setPropertyValue("ParentPath",newLinkPath);

	//Invalidate the linked Id.
	propertyContainer_->setPropertyValue("ParentId",-1);
}

void AssociateHostLink::postDeserialize()
{
	DataStore::postDeserialize();
}

bool AssociateHostLink::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Updates the values in the ParentPath and ParentId Properties to reflect the current stored linkedAsset_.
*/
void AssociateHostLink::updateLinkedAssetProperties()
{
	Q_ASSERT(!linkedAsset_.isNull());
	////When writing the parent path, we remove the components of the path up to the asset linked to 
	////the associated root.  For example, an Analysis tree links to a Task.  A tasks path is "EyeCalibration".
	////When saving the Parent Path, for an Analysis Element, we might find the parents path is:
	////"EyeCalibration::PresentStimuli::IncrementStimulusCounter".  Since the Tasks path contains only one
	////components, we remove the first component in this path, leaving "PresentStimuli::IncrementStimulusCounter"
	////and save this value.  This allows us to easily link Analysis Elements to other tasks with different names
	////but the same tree otherwise.
	//QSharedPointer<Asset> grandParent = getParentAsset()->getParentAsset();
	//Q_ASSERT(grandParent);
	//QSharedPointer<AssociateRoot> assocRoot = grandParent.staticCast<AssociateRoot>();
	//Q_ASSERT(assocRoot);
	//Q_ASSERT(assocRoot->getLinkedAsset());
	//int removeDepth = assocRoot->getLinkedAsset()->getPath().split("::").size();
	//QString assetPath = linkedAsset_.toStrongRef()->getPath();
	//QStringList elementsWithoutTask = assetPath.split("::");
	//for(int i=0;i<removeDepth;i++)
	//{
	//	elementsWithoutTask.removeAt(0);
	//}
	//QString storablePath = elementsWithoutTask.join("::");
	QString storablePath = linkedAsset_.toStrongRef()->getPath();
	propertyContainer_->setPropertyValue("ParentPath",storablePath);
	propertyContainer_->setPropertyValue("ParentId",linkedAsset_.toStrongRef()->getAssetId());
}

}; //namespace Picto
