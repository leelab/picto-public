#ifndef _ObsoleteNameAsset_H_
#define _ObsoleteNameAsset_H_

#include <QSharedPointer>

#include "../common.h"
#include "OldVersionAsset.h"
#include "DataStore.h"

namespace Picto {
/*!	\brief Used to upgrade the syntax of Picto designs where an object's XML tag name needs to change.
 *
 *	\details Inevitably, as we have developed, Picto bugs have cropped up and design decision changes have been made.  Sometimes this
 *	resulted in an element's XML tag name changing, even though the rest of the XML could stay the same.  In order to automatically
 *	upgrade a Picto design and fix this type of issue, in the new design syntax version (#define DESIGNSYNTAXVERSION), we set the 
 *	particular XML tag that needs to change to deserialize into an ObsoleteNameAsset.  After the design is deserialized, we can use 
 *	Asset::upgradeVersion() in the parent of the ObsoleteNameAsset to set a new tag name using setNewTagName().  By using 
 *	OldVersionAsset::reportOldVersionAsset(), we make sure that the design will then be serialized out and deserialized back in.  
 *	During serialization the ObsoleteNameAsset is saved with a new XML tag name.  During deserialization the XML has the correct tag 
 *	and fits the upgraded syntax.
 * 
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ObsoleteNameAsset : public DataStore, OldVersionAsset
#else
class ObsoleteNameAsset : public DataStore, OldVersionAsset
#endif
{
	Q_OBJECT
public:
	ObsoleteNameAsset();
	virtual ~ObsoleteNameAsset();
	static QSharedPointer<Asset> Create();
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void postDeserialize();
	virtual QString friendlyTypeName(){return "Obsolete Name Asset";};

	/*! \brief Gets this object's AssetId.*/
	virtual	int getAssetId(){return assetId_;};

	void setNewTagName(QString newName);
	QString getOriginalType();

private:
	QString newTagName_;
	QString tagText_;
	QString origType_;
	int assetId_;
};


}; //namespace Picto

#endif
