#ifndef _AssociateRootHost_H_
#define _AssociateRootHost_H_

#include <QSharedPointer>
#include "../common.h"
#include "AssociateRoot.h"

//Copy this into the public area of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)
#define ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION		virtual QUuid getHostId(){return propertyContainer_->getPropertyValue("HostId").toUuid();};	\
														virtual QSharedPointer<Property> getHostIdProperty(){return propertyContainer_->getProperty("HostId");};	\
														virtual QSharedPointer<Property> getNameProperty(){return propertyContainer_->getProperty("Name");};	
//Copy these lines (uncommented) into the private area of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)
//Note: The QT preprocess didn't like it when we put this in a #define and caused runtime errors, so you should just uncomment and copy the lines below.
	//bool hostIdBeingEdited_;
	//private slots:
	//void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};

//Copy this into the constructor of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)
#define ASSOCIATE_ROOT_HOST_INITIALIZATION	AddDefinableProperty(QVariant::Uuid,"HostId",QVariant());	\
											hostIdBeingEdited_ = false;
//Copy this to the end of  the postDeserialize() function of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)
//Note that this is setup such that whenever this object is edited, its HostId is changed to signify that Associate Elements that aren't currently
//attached to it may no longer be valid
#define ASSOCIATE_ROOT_HOST_POST_DESERIALIZE	if(getHostId() == QUuid())	\
													changeHostId();	\
												propertyContainer_->getProperty("HostId")->setVisible(false);	\
												connect(this,SIGNAL(edited()),this,SLOT(changeHostId()));

namespace Picto {
/*! \brief An AssociateRootHost connects to an AssociateRoot that provides additional information regarding the asset that is not relevant to the assets basic functionality.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateRootHost
#else
class AssociateRootHost
#endif
{

public:
	AssociateRootHost();
	virtual ~AssociateRootHost(){};
	QSharedPointer<AssociateRoot> getAssociateRoot(){return associateRoot_;};
	virtual QUuid getHostId() = 0;
	virtual QSharedPointer<Property> getHostIdProperty() = 0;
	virtual QSharedPointer<Property> getNameProperty() = 0;

private:
	QSharedPointer<AssociateRoot> associateRoot_;
	void setAssociateRoot(QSharedPointer<AssociateRoot> assocRoot){associateRoot_ = assocRoot;};
	friend class AssociateRoot;

};


}; //namespace Picto

#endif
