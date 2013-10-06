#ifndef _ObsoleteNameAsset_H_
#define _ObsoleteNameAsset_H_

#include <QSharedPointer>

#include "../common.h"
#include "OldVersionAsset.h"
#include "DataStore.h"

namespace Picto {
/*!	\brief Used to hold data from asset tags of assets that are not obsolete but have had their asset tag name changed
 * 
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

	virtual	int getAssetId(){return assetId_;};

	//Sets the tag name to which this objects tag should be changed
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
