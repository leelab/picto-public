#ifndef _AssociateElement_H_
#define _AssociateElement_H_

#include <QString>
#include <QUuid>
#include "../storage/Asset.h"
#include "AssociateRoot.h"
#include "AssociateHostLink.h"
#include "../storage/DataStore.h"
#include "../common.h"

namespace Picto {

//The #defines below implements the AssociateElement for its child classes in a default way.
//This is really just a simple way of cleaning up the multiple inheritance necessary
//to all AssociateElements to inherit from design elements but still have common properties.
#define ASSOCIATE_ELEMENT_IMPLEMENTATION 	virtual QString getParentPath()	\
											{								\
												QSharedPointer<AssociateHostLink> lnk = getGeneratedChildren("HostLink").first().staticCast<AssociateHostLink>();	\
												return lnk->getParentPath();\
											};	\
											\
											virtual int getParentId()	\
											{								\
												QSharedPointer<AssociateHostLink> lnk = getGeneratedChildren("HostLink").first().staticCast<AssociateHostLink>();	\
												return lnk->getParentId();\
											};	\
											virtual void linkToAsset(QSharedPointer<Asset> asset)	\
											{								\
												QSharedPointer<AssociateHostLink> lnk = getGeneratedChildren("HostLink").first().staticCast<AssociateHostLink>();	\
												lnk->linkToAsset(asset);	\
											};	\
											virtual AttachmentResult attachToLinkedAsset(bool tryLinkingById)	\
											{	\
												QSharedPointer<Asset> associateRoot = getParentAsset();	\
												Q_ASSERT(associateRoot);	\
												QSharedPointer<Asset> linkedAsset = associateRoot.staticCast<AssociateRoot>()->getLinkedAsset();	\
												Q_ASSERT(linkedAsset && linkedAsset->inherits("Picto::DataStore"));	\
												\
												if(tryLinkingById)	\
												{	\
													if(linkedAsset.staticCast<DataStore>()->AddAssociateChild(getAssociateId(),getParentId(),identifier(),selfPtr()))	\
														return SUCCESS_BY_LINK;	\
												}	\
												QString fullPath = linkedAsset->getPath() + "::" + getParentPath();	\
												if(linkedAsset.staticCast<DataStore>()->AddAssociateChild(getAssociateId(),fullPath,identifier(),selfPtr()))	\
													return SUCCESS_BY_PATH;	\
												return FAIL;	\
											};	\
											virtual QUuid getAssociateId()	\
											{								\
												QSharedPointer<Asset> associateRoot = getParentAsset();	\
												Q_ASSERT(associateRoot);	\
												return associateRoot.staticCast<AssociateRoot>()->getAssociateId();	\
											};	\
											virtual QString getInfoString()	\
											{	\
												return "Name: " + getName() + "\nType: " + identifier() + "\nLinked to: " + getParentPath();	\
											};	\
											virtual QSharedPointer<Asset> getLinkedAsset()	\
											{	\
												QSharedPointer<AssociateHostLink> lnk = getGeneratedChildren("HostLink").first().staticCast<AssociateHostLink>();	\
												return lnk->getLinkedAsset();	\
											};

#define EXP_LINK_FACTORY_CREATION AddDefinableObjectFactory("HostLink",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AssociateHostLink::Create))));

/*! \brief Interface to AssociateRoot Elements
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateElement
#else
class AssociateElement
#endif
{

public:
	AssociateElement(){};
	virtual ~AssociateElement(){};

	virtual QString getParentPath() = 0;
	virtual int getParentId() = 0;
	virtual void linkToAsset(QSharedPointer<Asset> asset) = 0;
	enum AttachmentResult { SUCCESS_BY_LINK,SUCCESS_BY_PATH,FAIL };
	virtual AttachmentResult attachToLinkedAsset(bool tryLinkingById) = 0;
	virtual QUuid getAssociateId() = 0;
	virtual QString getInfoString() = 0;
	virtual QSharedPointer<Asset> getLinkedAsset() = 0;

private:
	QUuid AssociateRootId_;
};


}; //namespace Picto

#endif
