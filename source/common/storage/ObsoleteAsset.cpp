#include "ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

bool ObsoleteAsset::hadObsoleteAsset_ = false;

ObsoleteAsset::ObsoleteAsset() :
tagName_(""),
assetId_(0)
{
	hadObsoleteAsset_ = true;
}

ObsoleteAsset::~ObsoleteAsset()
{
}

QSharedPointer<Asset> ObsoleteAsset::Create()
{
	return QSharedPointer<Asset>(new ObsoleteAsset());
}

bool ObsoleteAsset::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	Q_ASSERT_X(false,"ObsoleteAsset::serializeAsXml","An ObsoleteAsset's serializeAsXml() function should never be used.");
	return true;
}
bool ObsoleteAsset::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader,bool)
{
	//Get Start tag name (depending on who the parent is, they may have given us different names.
	tagName_ = xmlStreamReader->name().toString();
	foreach(QXmlStreamAttribute xmlStreamAttr,xmlStreamReader->attributes())
	{
		attrMap_[xmlStreamAttr.name().toString()] = xmlStreamAttr.value().toString();
	}
	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == tagName_) && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}
		QSharedPointer<ObsoleteAsset> child(new ObsoleteAsset());
		child->deserializeFromXml(xmlStreamReader,false);
		children_.push_back(child);
		xmlStreamReader->readNext();
	}
	return true;
}
void ObsoleteAsset::postDeserialize()
{
	setDeleted();
	emit edited();
}

QSharedPointer<ObsoleteAsset> ObsoleteAsset::getChildAsset(int index)
{
	if(index >= numChildAssets())
		return QSharedPointer<ObsoleteAsset>();
	return children_[index];
}

QString ObsoleteAsset::getAttributeName(int index)
{
	if(index >= attrMap_.size())
		return "";
	return attrMap_.keys()[index];
}

QString ObsoleteAsset::getAttributeValue(QString name)
{
	if(!attrMap_.contains(name))
		return "";
	return attrMap_.value(name);
}

}; //namespace Picto
