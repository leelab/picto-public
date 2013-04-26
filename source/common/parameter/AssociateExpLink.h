#ifndef _AssociateExpLink_H_
#define _AssociateExpLink_H_

#include <QSharedPointer>
#include "../common.h"
#include "../storage/asset.h"
#include "Parameter.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateExpLink : public Parameter
#else
class AssociateExpLink : public Parameter
#endif
{
	Q_OBJECT

public:
	AssociateExpLink();
	virtual ~AssociateExpLink(){};
	static QSharedPointer<Asset> Create();

	QString getParentPath(){return propertyContainer_->getPropertyValue("ParentPath").toString();};
	int getParentId(){return propertyContainer_->getPropertyValue("ParentId").toInt();};
	QSharedPointer<Asset> getLinkedAsset(){return linkedAsset_.toStrongRef();};
	void linkToAsset(QSharedPointer<Asset> asset);

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
