#include "AnalysisExpLink.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisExpLink::AnalysisExpLink()
: Parameter()
{
	AddDefinableProperty(QVariant::String,"ParentPath",QVariant());
	AddDefinableProperty(QVariant::Int,"ParentId",0);
}

QSharedPointer<Asset> AnalysisExpLink::Create()
{
	QSharedPointer<AnalysisExpLink> newAnalysisExpLink(new AnalysisExpLink());
	newAnalysisExpLink->setSelfPtr(newAnalysisExpLink);
	return newAnalysisExpLink;
}

void AnalysisExpLink::linkToAsset(QSharedPointer<Asset> asset)
{
	Q_ASSERT(asset);
	if(linkedAsset_ == asset)
		return;
	//If this link was already connected to an asset, disconnect its name and id changed signals
	if(linkedAsset_)
	{
		connect(linkedAsset_.data(),SIGNAL(assetIdEdited()),this,SLOT(updateLinkedAssetProperties()));
		connect(linkedAsset_.staticCast<UIEnabled>().data(),SIGNAL(nameEdited()),this,SLOT(updateLinkedAssetProperties()));		
	}
	linkedAsset_ = asset;
	//Connect to the assets name and id changed signals so that this link will be updated if they change.
	connect(linkedAsset_.data(),SIGNAL(assetIdEdited()),this,SLOT(updateLinkedAssetProperties()));
	connect(linkedAsset_.staticCast<UIEnabled>().data(),SIGNAL(nameEdited()),this,SLOT(updateLinkedAssetProperties()));		
	updateLinkedAssetProperties();
}

void AnalysisExpLink::postDeserialize()
{
	Parameter::postDeserialize();
}

bool AnalysisExpLink::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

void AnalysisExpLink::updateLinkedAssetProperties()
{
	QString assetPath = linkedAsset_->getPath();
	QStringList elementsWithoutTask = assetPath.split("::");
	elementsWithoutTask.removeAt(0);
	QString tasklessPath = elementsWithoutTask.join("::");
	propertyContainer_->setPropertyValue("ParentPath",tasklessPath);
	propertyContainer_->setPropertyValue("ParentId",linkedAsset_->getAssetId());
}

}; //namespace Picto
