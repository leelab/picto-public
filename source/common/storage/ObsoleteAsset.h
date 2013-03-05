#ifndef _OBSOLETEASSET_H_
#define _OBSOLETEASSET_H_

#include <QSharedPointer>

#include "../common.h"
#include "../storage/Asset.h"

namespace Picto {
/*!	\brief Used to hold data from obsolete asset tags in an experiment file for autoupgrade purposes
 * 
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ObsoleteAsset : public Asset
#else
class ObsoleteAsset : public Asset
#endif
{
	Q_OBJECT
public:
	ObsoleteAsset();
	virtual ~ObsoleteAsset();
	static QSharedPointer<Asset> Create();
	static bool encounteredObsoleteAsset(){return hadObsoleteAsset_;};
	static void clearObsoleteAssetFlag(){hadObsoleteAsset_ = false;};
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader, bool validate);
	virtual void postDeserialize();

	virtual QString identifier(){return tagName_;};
	virtual QString assetType(){return "ObsoleteAsset";};

	virtual	int getAssetId(){return assetId_;};
	virtual void setAssetId(int id){assetId_ = id;};

	int numChildAssets(){return children_.size();};
	QSharedPointer<ObsoleteAsset> getChildAsset(int index);

	int numAttributes(){return attrMap_.size();};
	QString getAttributeName(int index);
	QString getAttributeValue(QString name);
	
	QString getValue(){return value_;};
private:
	QString tagName_;
	QMap<QString,QString> attrMap_;
	QVector<QSharedPointer<ObsoleteAsset>> children_;
	QString value_;
	int assetId_;
	static bool hadObsoleteAsset_;
};


}; //namespace Picto

#endif
