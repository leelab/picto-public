#include "UIData.h"
#include "UIElement.h"
#include "../design/PictoData.h"
#include "../experiment/Experiment.h"
#include "../parameter/Analysis.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new UIData object
 *	\details Adds an OpenAsset Property to hold the AssetId of the Asset that is currently open in the StateMachine Editor.
 *	Also adds an AssetFactory for creating UIElement children for this UIData object.
 */
UIData::UIData()
:	AssociateRoot(),
	elementFactory_(new AssetFactory(0,-1))
{
	AddDefinableProperty(QVariant::Int,"OpenAsset",0);

	AddDefinableObjectFactory("UIElement",elementFactory_);
	elementFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(UIElement::Create))));
}

/*! \brief Creates a new UIData object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> UIData::Create()
{
	QSharedPointer<UIData> newUIData(new UIData());
	newUIData->setSelfPtr(newUIData);
	return newUIData;
}

/*! \brief Sets the assetId of the Asset that is opened in the StateMachine Editor.*/
void UIData::setOpenAsset(int assetId)
{
	propertyContainer_->setPropertyValue("OpenAsset",assetId);
}

/*! \brief Returns the assetId of the Asset that should be opened in the StateMachine Editor.*/
int UIData::getOpenAsset()
{
	return propertyContainer_->getPropertyValue("OpenAsset").toInt();
}

void UIData::postDeserialize()
{
	AssociateRoot::postDeserialize();
}

bool UIData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AssociateRoot::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

