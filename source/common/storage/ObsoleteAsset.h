#ifndef _OBSOLETEASSET_H_
#define _OBSOLETEASSET_H_

#include <QSharedPointer>

#include "../common.h"
#include "OldVersionAsset.h"
#include "Asset.h"

namespace Picto {
/*!	\brief Used to hold data from XML tags representing Assets that are now obsolete.
 *	
 *	\details Inevitably, as we have developed Picto bugs have cropped up and design decision changes have been made; however,
 *	since Sessions had already been run and designs built, we needed a way to be able to automatically upgrade old syntax and still
 *	be able to playback old sessions.  Part of this system is the ObsoleteAsset class.  When design syntax changes need to be made, 
 *	we increase the \#define DESIGNSYNTAXVERSION, then we set the particular XML tag for the Asset type that is becoming obsolete to 
 *	deserialize into an ObsoleteAsset.  After the design is deserialized, we can use Asset::upgradeVersion() in the parent of the 
 *	ObsoleteAsset to extract any important data from the ObsoleteAsset and put it somewhere where it will be needed in the upgraded
 *	design syntax.  
 *
 *	Data that is deserialized into the ObsoleteAsset can be extracted using getAttributeName(), getAttributeValue(), getValue(), etc functions.
 *	Also, if the XML of the ObsoleteAsset contains other XML tags, each of those will be deserialized into its own child ObsoleteAsset which 
 *	is accessible from getObsoleteChildAsset().
 * 
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ObsoleteAsset : public Asset, OldVersionAsset
#else
class ObsoleteAsset : public Asset, OldVersionAsset
#endif
{
	Q_OBJECT
public:
	ObsoleteAsset();
	virtual ~ObsoleteAsset();
	static QSharedPointer<Asset> Create();
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void postDeserialize();

	virtual QString identifier(){return tagName_;};
	virtual QString friendlyTypeName(){return "Obsolete Asset";};

	/*! \brief Returns the AssetId set in setAssetId().*/
	virtual	int getAssetId(){return assetId_;};
	/*! \brief Implements Asset::setAssetId() to store the input AssetId.*/
	virtual void setAssetId(int id){assetId_ = id;};

	/*! \brief Returns a list of the names of the xml tags that were included in the XML tag used to deserialize this ObsoleteAsset.*/
	QStringList childTags(){return children_.keys();};
	QList<QSharedPointer<ObsoleteAsset>> getObsoleteChildAsset(QString tagName);

	/*! \brief Returns the number of attributes that were included in the XML tag from which this ObsoleteAsset was deserialized.
	*/
	int numAttributes(){return attrMap_.size();};
	QString getAttributeName(int index);
	QString getAttributeValue(QString name);
	
	/*! \brief Returns the value contained in the ObsoleteAsset (between the open and closing xml tags for cases where it has no children).
	*/
	QString getValue(){return value_;};
private:
	QString tagName_;
	QMap<QString,QString> attrMap_;
	QMultiMap<QString,QSharedPointer<ObsoleteAsset>> children_;	//Maps children by tag name
	QString value_;
	int assetId_;
};


}; //namespace Picto

#endif
