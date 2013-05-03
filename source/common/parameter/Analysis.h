#ifndef _Analysis_H_
#define _Analysis_H_

#include "../common.h"
#include "AssociateRoot.h"
#include "AssociateRootHost.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Analysis : public AssociateRoot, public AssociateRootHost
#else
class Analysis : public AssociateRoot, public AssociateRootHost
#endif
{
	Q_OBJECT
public:
	Analysis();
	virtual ~Analysis(){};
	static QSharedPointer<Asset> Create();
	virtual QString friendlyTypeName(){return "Analysis";};

	ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "Analysis";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<AssetFactory> variableFactory_;
	QSharedPointer<AssetFactory> dataSourceFactory_;
	QSharedPointer<AssetFactory> outputFactory_;
	QSharedPointer<AssetFactory> functionFactory_;
	QSharedPointer<AssetFactory> scriptFactory_;

	bool hostIdBeingEdited_;
private slots:
	void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};


};


}; //namespace Picto

#endif
