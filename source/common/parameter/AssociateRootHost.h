#ifndef _AssociateRootHost_H_
#define _AssociateRootHost_H_

#include <QSharedPointer>
#include <QHash>
#include "AssociateRoot.h"
#include "../common.h"

/*! \file */
/*! \def ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION		
 *	\brief Put this into the public area of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)*/
#define ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION		virtual QUuid getHostId(){return propertyContainer_->getPropertyValue("HostId").toUuid();};	\
														virtual QSharedPointer<Property> getHostIdProperty(){return propertyContainer_->getProperty("HostId");};	\
														virtual QSharedPointer<Property> getNameProperty(){return propertyContainer_->getProperty("Name");};	

/*! Copy the lines of code below into the private area of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)
 *	Note: The QT preprocess didn't like it when we put this in a \#define and caused runtime errors, so you should just uncomment and copy the lines below.

	bool hostIdBeingEdited_;
	private slots:
	void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};
 
 *
 */

/*! \def ASSOCIATE_ROOT_HOST_INITIALIZATION	
 *	\brief Put this into the constructor of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)*/
#define ASSOCIATE_ROOT_HOST_INITIALIZATION	AddDefinableProperty(QVariant::Uuid,"HostId",QVariant());	\
											hostIdBeingEdited_ = false;
/*! \def ASSOCIATE_ROOT_HOST_POST_DESERIALIZE	
 *	\brief Put this at the end of  the postDeserialize() function of your AssociateRootHost (this is to avoid diamond multiple inheritance issues)
 *	\note This is setup such that whenever this object is edited, its HostId is changed to signify that Associate Elements that aren't currently
 *	attached to it may no longer be valid.
 */
#define ASSOCIATE_ROOT_HOST_POST_DESERIALIZE	if(getHostId() == QUuid())	\
													changeHostId();	\
												propertyContainer_->getProperty("HostId")->setVisible(false);	\
												connect(this,SIGNAL(edited()),this,SLOT(changeHostId()));

namespace Picto {
/*! \brief An AssociateRootHost object's tree can be linked to by an AssociateRoot object's tree.  Elements in the AssociateRoot tree connect to elements in the 
 *	AssociateRootHost tree and provide additional information about them that does not affect their basic functionality.
 *	\details By separating out the AssociateRootHost from the AssociateRoot, we can add or remove AssociateRoots without affecting the integrity of the 
 *	AssociateRootHost and we can keep its size and complexity down to a minimum.  The idea is that AssociateRoots link to AssociateRootHosts.  Each AssociateRootHost
 *	has a HostId and each AssociateRoot stores the HostId of the AssociateRootHost to which it is linked.  Whenever a change occurs in the AssociateRootHost, its
 *	HostId changes, and any AssociateRoots that are attached to the AssociateRootHost at the time get their stored HostId's changed too such that we can always tell if
 *	an AssociateRoot is a "perfect match" for an AssociateRootHost if their HostId's match up.  In the case where the HostIds differ, there is also the option
 *	of attempting to import an AssociateRoot into and AssociateRootHost; however, since the match wasn't perfect we will know that the resulting import
 *	needs to be viewed with caution in case some of the logic is no longer valid.
 *	
 *	In order to avoid diamond inheritance issues (a class inherits two classes that both inherit from the same class) we had to cut some corners here.
 *	we wanted classes that are already QObjects to inherit from AssociateRootHost but this would create diamond inheritance if AssociateRootHost was also
 *	a QObject and we needed AssociateRootHost to inherit from UIEnabled as well, which is a QObject.  
 *	This means that we couldn't make AssociateRootHost a QObject.  Instead, in order to use some signal/slot functionality that can only
 *	be used in QObjects we provide some \#defines and commented code in this file that need to be copied into any class that wants to be an AssociateRootHost.
 *	
 *	To implement an AssociateRootHost, do as follows:
 *		- Inherit from AssociateRootHost and UIEnabled or something that inherits from UIEnabled.
 *		- Put ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION into the public area of your AssociateRootHost's .h file.
 *		- Put ASSOCIATE_ROOT_HOST_INITIALIZATION into the constructor of your AssociateRootHost.
 *		- Put ASSOCIATE_ROOT_HOST_POST_DESERIALIZE at the end of the postDeserialize() function of your AssociateRootHost.
 *		- Copy the following lines of code into the private area of your AssociateRootHost's .h file:
 *	\code
	
		bool hostIdBeingEdited_;
		private slots:
		void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};
	
	\endcode
 *	\details These copied lines take care of automatically changing the HostId whenever the AssociateRootHost's tree is edited and make the data in its "Name" Property
 *	accessible so that the AssociateRoot can record it.
 *	\note The reason that we didn't put the code that goes into the private area of the AssociateRootHost's .h file in a \#define is that the QT preprocessor
 *	didn't properly understand it when we did this and it led to runtime errors.
 *	\sa AssociateRoot, AssociateElement, Analysis, UIData, Experiment
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Returns the AssociateRoot of the input type that is linked to this AssociateRootHost.*/
	QSharedPointer<AssociateRoot> getAssociateRoot(QString type){if(!associateRootHash_.contains(type))return QSharedPointer<AssociateRoot>();return associateRootHash_.value(type);};
	/*! \brief Returns this AssociateRootHost's HostId.  
	 *	\details The HostId changes whenever the AssociateRootHost's tree is edited and is a Uuid such that each id uniquely identifys this AssociateRootHost and its current state.
	 */
	virtual QUuid getHostId() = 0;
	/*! \brief Returns a pointer to the Property containing this object's HostId.
	 *	\details This is useful in case an AssociateRoot wants to be updated whenever the HostId changes.
	 */
	virtual QSharedPointer<Property> getHostIdProperty() = 0;
	/*! \brief Returns a pointer to the Property containing this object's name.
	 *	\details This is useful in case an AssociateRoot wants to be updated whenever this object's name changes.
	 */
	virtual QSharedPointer<Property> getNameProperty() = 0;

private:
	QHash<QString,QSharedPointer<AssociateRoot>> associateRootHash_;
	/*! \brief Adds the input AssociateRoot to this AssociateRootHost's AssociateRoot lookup table under the input type name.
	 *	\note This function's implementation only allows a single AssociateRoot for each input type.  This seems to be a bug.
	 *	It makes sense that each AssociateRootHost would only have one linked UIData AssociateRoot for example; however, an
	 *	Experiment can have multiple Analysis AssociateRoots attached to it and running simultaneously.  THis has not caused
	 *	problems yet since this function only adds the AssociateRoot to an associateRootHash_ and that hash appears to only be
	 *	used for accessing attached UIData.  We should probably spend some more time verifying this issue though and fixing it.
	 */
	void setAssociateRoot(QString type, QSharedPointer<AssociateRoot> assocRoot){associateRootHash_[type] = assocRoot;};
	friend class AssociateRoot;

};


}; //namespace Picto

#endif
