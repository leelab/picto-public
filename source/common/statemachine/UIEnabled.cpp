#include "UIEnabled.h"
#include "../parameter/AssociateRootHost.h"
#include "../parameter/AssociateRoot.h"
#include "../storage/ObsoleteAsset.h"
#include "../statemachine/UIElement.h"
#include "../memleakdetect.h"
namespace Picto
{

UIEnabled::UIEnabled()
: DataStore()
{
	AddDefinableProperty("Name","NotYetNamed");
	AddDefinableObjectFactory("UIInfo",
			QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	requireUniqueName();
}

QSharedPointer<Asset> UIEnabled::Create()
{
	return QSharedPointer<Asset>(new UIEnabled());
}

void UIEnabled::setPos(QPoint pos)
{
	//Get the UI Element
	QSharedPointer<Asset> uiElement = getUIElement();
	//Set my position in the UIElement.
	uiElement.staticCast<UIElement>()->setPos(pos);
}

QPoint UIEnabled::getPos()
{
	//Get the UI Element
	QSharedPointer<Asset> uiElement = getUIElement();
	return uiElement.staticCast<UIElement>()->getPos();

}

void UIEnabled::setNotes(QString notes)
{
	//Get the UI Element
	QSharedPointer<Asset> uiElement = getUIElement();
	//Set the notes in the UIElement
	uiElement.staticCast<UIElement>()->setNotes(notes);
}

QString UIEnabled::getNotes()
{
	//Get the UI Element
	QSharedPointer<Asset> uiElement = getUIElement();
	//Get the notes from the UIElement
	return uiElement.staticCast<UIElement>()->getNotes();
}

void UIEnabled::upgradeVersion(QString deserializedVersion)
{
	DataStore::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{
		//Before Design syntax version 0.0.1, all UI position data was stored in UIInfo objects that were sub-elements
		//of UIEnabled objects.  At version 0.0.1, we moved UI data out of the Experimental design into a separate UIData design.
		
		//Get the old position from the obsolete UIInfo asset
		QList<QSharedPointer<Asset>> oldUIInfoList = getGeneratedChildren("UIInfo");
		Q_ASSERT(oldUIInfoList.size() <=  1);
		if(oldUIInfoList.isEmpty())
			return;	//Looks like there's nothing to do here.
		QSharedPointer<ObsoleteAsset> oldPosAsset = oldUIInfoList.first().staticCast<ObsoleteAsset>()->getObsoleteChildAsset("Pos").first().staticCast<ObsoleteAsset>();
		//Check if the position data is stored as attributes or a tag value (this has been updated to always use tag values in new versions)
		//and set it to a position variable.
		QPoint pos;
		if(oldPosAsset->numAttributes()<=1)
		{
			QStringList values = oldPosAsset->getValue().split(",");
			pos = QPoint(values[0].toInt(),values[1].toInt());
		}
		else
		{
			pos = QPoint(oldPosAsset->getAttributeValue("x").toInt(),oldPosAsset->getAttributeValue("y").toInt());
		}
		//Set the position to a UIElement in the newly created UIData object
		setPos(pos);
	}
}

void UIEnabled::preSerialize()
{
	DataStore::preSerialize();
}

void UIEnabled::postDeserialize()
{
	DataStore::postDeserialize();
	QSharedPointer<Property> nameProp = propertyContainer_->getProperty("Name");
	if(nameProp)
		connect(nameProp.data(),SIGNAL(edited()),this,SIGNAL(nameEdited()));
}


bool UIEnabled::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	QSharedPointer<Property> nameProp = propertyContainer_->getProperty("Name");
	if(nameProp && nameProp->value().toString().isEmpty())
	{
		QString errMsg = QString("Asset can not have an empty name.");
		addError(errMsg);
		return false;
	}
	if(nameProp && nameProp->value().toString().contains("::"))
	{
		addError("Element names cannot contain '::'.");
		return false;
	}
	return true;
}


bool UIEnabled::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(DataStore::executeSearchAlgorithm(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::STRING:
		{
			//Check if my name includes the search string
			if(getName().contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
				return true;
		}
		break;
	default:
		return false;
	};
	return false;
}

QSharedPointer<Asset> UIEnabled::getUIElement()
{
	//If we've already done this before, use the UIData associate ID that we got last time to find the UIElement
	QList<QSharedPointer<Asset>> assocChildren;
	if(!uiAssociateId_.isNull())
	{
		assocChildren = getAssociateChildren(uiAssociateId_,"UIElement");
		if(assocChildren.size())
		{
			return assocChildren.first();
		}
	}

	//If we got here, we don't have a stored UIData associate ID that has a UIElement in it.  Find one and get the element.
	foreach(QUuid associateId,getAttachedAssociateIds())
	{
		assocChildren = getAssociateChildren(associateId,"UIElement");
		if(assocChildren.size())
		{
			uiAssociateId_ = associateId;
			return assocChildren.first();
		}
	}

	//If we got here, there is no attached UIElement, create one in the UIData attached to the root of this tree

	//Get the root of this tree, beneath the PictoData object.  It will be an AssociateRootHost.
	QSharedPointer<Asset> ancestor = selfPtr();
	while(ancestor->getParentAsset() && ancestor->getParentAsset()->getParentAsset())
	{
		ancestor = ancestor->getParentAsset();
	}
	//Get the AssociateRoot attached to it.  This will be a UIData object.
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(ancestor.data());
	Q_ASSERT(assocRootHost);
	QSharedPointer<AssociateRoot> uiData = assocRootHost->getAssociateRoot("UIData");
	//Store the uiAssociateId of the UIData object
	uiAssociateId_ = uiData->getAssociateId();
	//Add a new UIElement to the data object and link it to me.
	QSharedPointer<Asset> uiElement = uiData->createChildAsset("UIElement",QString(),QString());
	AddAssociateChild(uiAssociateId_,"UIElement",uiElement);
	
	//Return the newly generated element
	return uiElement;
}
}//namespace Picto