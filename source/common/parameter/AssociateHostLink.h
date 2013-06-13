#ifndef _AssociateHostLink_H_
#define _AssociateHostLink_H_

#include <QSharedPointer>
#include "../common.h"
#include "../storage/asset.h"
#include "../storage/DataStore.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateHostLink : public DataStore
#else
class AssociateHostLink : public DataStore
#endif
{
	Q_OBJECT

public:
	AssociateHostLink();
	virtual ~AssociateHostLink(){};
	static QSharedPointer<Asset> Create();

	QString getParentPath(){return propertyContainer_->getPropertyValue("ParentPath").toString();};
	int getParentId(){return propertyContainer_->getPropertyValue("ParentId").toInt();};
	QSharedPointer<Asset> getLinkedAsset(){return linkedAsset_.toStrongRef();};
	void linkToAsset(QSharedPointer<Asset> asset);
	void updateLinkPath(QString oldPrefix,QString newPrefix);

signals:
	void linkedToAsset(QSharedPointer<Asset> asset);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QWeakPointer<Asset> linkedAsset_;
private slots:
	void updateLinkedAssetProperties();
};


}; //namespace Picto

#endif
