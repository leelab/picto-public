#include "AssociateRootHost.h"
#include "AssociateRoot.h"
#include "../design/PictoData.h"
#include "AssociateElement.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

AssociateRoot::AssociateRoot()
:	ScriptableContainer(),
	variableFactory_(new AssetFactory(0,-1)),
	outputFactory_(new AssetFactory(0,-1)),
	dataSourceFactory_(new AssetFactory(0,-1)),
	functionFactory_(new AssetFactory(0,-1)),
	scriptFactory_(new AssetFactory(0,-1))
{
	AddDefinableProperty(QVariant::Uuid,"AssociateId",QVariant());
	AddDefinableProperty(QVariant::Uuid,"LinkedHostId",QVariant());
	AddDefinableProperty(QVariant::String,"LinkedHostName",QVariant());

	requireUniqueName(false);
}

//Links this AssociateRoot to the input Asset.
//Returns true on success, false if the linkage wasn't perfect.  On false, the feedback
//message has more information.
//THIS IS GOING TO NEED SOME DEBUGGING.  MAKE SURE, FOR EXAMPLE, THAT ADDING AN ANALYSISSCRIPTCONTAINER TO A
//NUMERIC PARAMETER WON'T BREAK ANYTHING!!!  Possibly fix these types of issues by overloading the AssociateElement's
//attachToLinkedAsset function for different types of classes depending on whether they can be loaded onto the wrong
//element or not.
bool AssociateRoot::LinkToAsset(QSharedPointer<Asset> asset, QString& feedback)
{
	if(asset == linkedAsset_.toStrongRef())
		return true;

	feedback = "";
	AssociateElement* associateElement;
	bool linkedAssetMatches = isLinkableAsset(asset);
	QStringList pathMatchedAssociateRootAssets;
	QStringList unmatchedAssociateRootAssets;
	//Set this asset as our linked asset
	setLinkedAsset(asset);
	foreach(QString childTag,getValidChildTags())
	{
		foreach(QSharedPointer<Asset> child,getGeneratedChildren(childTag))
		{
			associateElement = dynamic_cast<AssociateElement*>(child.data());
			if(!associateElement || child->isDeleted())
				continue;
			
			int result = associateElement->attachToLinkedAsset(linkedAssetMatches);	//Linked Asset needs to be stored before we call this, since associateElement gets it from us. 
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
	ScriptableContainer::postDeserialize();
	if(getAssociateId() == QUuid())
		propertyContainer_->setPropertyValue("AssociateId",QUuid::createUuid());
	setDescendantPropertiesAsAssociates();
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(AssociateRootChildWasAdded(QSharedPointer<Asset>)));
}

bool AssociateRoot::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

void AssociateRoot::setLinkedAsset(QSharedPointer<Asset> asset)
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
	assocRootHost->setAssociateRoot(identifier(),selfPtr().staticCast<AssociateRoot>());
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

void AssociateRoot::updateLinkedAssetProperties()
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);
	propertyContainer_->setPropertyValue("LinkedHostId",assocRootHost->getHostId());
	propertyContainer_->setPropertyValue("LinkedHostName",linkedAsset_.toStrongRef()->getName());
}

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
QSharedPointer<Property> AssociateRoot::getLinkedHostNameProperty()
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);
	return assocRootHost->getNameProperty();
}

QSharedPointer<Property> AssociateRoot::getLinkedAssetHostIdProperty()
{
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(linkedAsset_.data());
	Q_ASSERT(assocRootHost);
	return assocRootHost->getHostIdProperty();
}

void AssociateRoot::linkedAssetPropertyEdited(Property*,QVariant)
{
	updateLinkedAssetProperties();
}

//Whenever a child is added to the AssociateRoot, we must set all of its descendant properties to Associate properties.
void AssociateRoot::AssociateRootChildWasAdded(QSharedPointer<Asset> child)
{
	if(!child->inherits("Picto::DataStore"))
		return;
	child.staticCast<DataStore>()->setDescendantPropertiesAsAssociates();
}

}; //namespace Picto
