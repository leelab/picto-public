#include "ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

ObsoleteAsset::ObsoleteAsset() :
tagName_(""),
value_(""),
assetId_(0)
{
}

ObsoleteAsset::~ObsoleteAsset()
{
}

/*! \brief Constructs and returns a shared pointer to a new ObsoleteAsset.
 */
QSharedPointer<Asset> ObsoleteAsset::Create()
{
	return QSharedPointer<Asset>(new ObsoleteAsset());
}

/*! \brief Implements Asset::serializeAsXml() to do nothing since ObsoleteAsset values are obsolete and never serialized
 *	out.
 */
bool ObsoleteAsset::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	Q_ASSERT_X(false,"ObsoleteAsset::serializeAsXml","An ObsoleteAsset's serializeAsXml() function should never be used.");
	return true;
}

/*! \brief Implements Asset::deserializeFromXml() to deserialize everything from the input XML tag to its closing tag into
 *	a "tree" of ObsoleteAsset objects, each containing a list of XML tag attributes, and the tag values.
 *	\details This function also uses reportOldVersionAsset() to inform Picto that something that was obsolete was
 *	serialized in so it will have to perform an automatic upgrade.
 */
bool ObsoleteAsset::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	reportOldVersionAsset();

	//Get Start tag name (depending on who the parent is, they may have given us different names.
	tagName_ = xmlStreamReader->name().toString();
	foreach(QXmlStreamAttribute xmlStreamAttr,xmlStreamReader->attributes())
	{
		attrMap_[xmlStreamAttr.name().toString()] = xmlStreamAttr.value().toString();
	}
	xmlStreamReader->readNext();
	value_ = xmlStreamReader->text().toString();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == tagName_) && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}
		QSharedPointer<ObsoleteAsset> child(new ObsoleteAsset());
		child->deserializeFromXml(xmlStreamReader);
		children_.insert(xmlStreamReader->name().toString(),child);
		xmlStreamReader->readNext();
	}
	return true;
}

/*! \brief Extends Asset::postDeserialize() to set this object deleted and emit the edited() signal to tell Parent Assets
 *	that this ObsoleteAsset won't be saved out.
 */
void ObsoleteAsset::postDeserialize()
{
	Asset::postDeserialize();
	setDeleted();
	emit edited();
}

/*! \brief Returns the child ObsoleteAsset that was serialized in with the input tagName.
 *	\sa deserializeFromXml()
 */
QList<QSharedPointer<ObsoleteAsset>> ObsoleteAsset::getObsoleteChildAsset(QString tagName)
{
	if(!children_.contains(tagName))
		return QList<QSharedPointer<ObsoleteAsset>>();
	return children_.values(tagName);
}

/*! \brief Returns the attribute name that was included in this ObsoleteAsset's XML tag at the input index.
 *	\sa numAttributes()
 */
QString ObsoleteAsset::getAttributeName(int index)
{
	if(index >= attrMap_.size())
		return "";
	return attrMap_.keys()[index];
}

/*! \brief Returns the attribute value that was included in this ObsoleteAsset's XML tag at the input index.
 *	\sa numAttributes()
 */
QString ObsoleteAsset::getAttributeValue(QString name)
{
	if(!attrMap_.contains(name))
		return "";
	return attrMap_.value(name);
}

}; //namespace Picto
