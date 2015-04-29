#ifndef _AssociateRoot_H_
#define _AssociateRoot_H_

#include "../common.h"
#include "../StateMachine/UIEnabled.h"

namespace Picto {

/*! \brief AssociateRoot objects are the root of a tree with elements that connect to an AssociateRootHost object's tree
 *	and provide additional information about the AssociateRootHost tree that does not affect its basic functionality.
 *	\details By separating out the AssociateRoot from the AssociateRootHost, we can add or remove AssociateRoot objects
 *	without affecting the integrity of the AssociateRootHost and we can keep its size and complexity down to a minimum.
 *	The idea is that AssociateRoots link to AssociateRootHosts.  Each AssociateRootHost has a HostId and each
 *	AssociateRoot stores the HostId of the AssociateRootHost to which it is linked.  Whenever a change occurs in the
 *	AssociateRootHost, its HostId changes, and any AssociateRoots that are attached to the AssociateRootHost at the time
 *	get their stored HostId's changed too such that we can always tell if an AssociateRoot is a "perfect match" for an
 *	AssociateRootHost if their HostId's match up.  In the case where the HostIds differ, there is also the option of
 *	attempting to import an AssociateRoot into and AssociateRootHost; however, since the match wasn't perfect we will
 *	know that the resulting import needs to be viewed with caution in case some of the logic is no longer valid.
 *	
 *	Like AssociateRootHosts, AssociateRoots also have an ID.  When AssociateElement children of an AssociateRoot are
 *	linked to elements in an AssociateRootHost's tree, they are linked according to their AssociateRoot parent's ID.
 *	This way, even if many different AssociateRoots are attached to a single AssociateRootHost, we can quickly enable
 *	and disable them by simply telling the AssoicateRootHost to pay attention to or ignore AssociateElements with
 *	particular Assoicate IDs.
 *
 *	\sa AssociateRoot, AssociateElement, Analysis, UIData, Experiment
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateRoot : public UIEnabled
#else
class AssociateRoot : public UIEnabled
#endif
{
	Q_OBJECT

public:
	AssociateRoot();
	virtual ~AssociateRoot(){};
	virtual QString friendlyTypeName(){return "Associate Root";};
	/*! \brief Returns this object's Associate ID.  All AssociateElement children of this object are linked to an
	 *	AssociateRootHost's descendants according to this ID.
	 */
	QUuid getAssociateId(){return propertyContainer_->getPropertyValue("AssociateId").toUuid();};
	/*! \brief Returns the Host ID of the AssociateRootHost that this AssociateRoot is linked to.  If no AssociateRootHost
	 *	is linked, returns an invalid QUuid.
	 */
	QUuid getLinkedHostId(){return propertyContainer_->getPropertyValue("LinkedHostId").toUuid();};
	/*! \brief Returns the name of the AssociateRootHost that this AssociateRoot is linked to.  If no AssociateRootHost
	 *	is linked, returns an empty name.
	 */
	QString getLinkedHostName(){return propertyContainer_->getPropertyValue("LinkedHostName").toString();};
	bool LinkToAsset(QSharedPointer<Asset> asset, QString& feedback, bool tempAsset = false);
	/*! \brief Returns the AssociateRootHost to which this object is linked as an Asset shared pointer.
	 *	\note This should probably be renamed to getLinkedHost() or something like that.  It is named this way for
	 *	historical reasons.
	 */
	QSharedPointer<Asset> getLinkedAsset(){return linkedAsset_;};
	QSharedPointer<Asset> getLinkableAsset();

protected:
	virtual QString defaultTagName(){return "AssociateRoot";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	void setLinkedAsset(QSharedPointer<Asset> asset, bool tempAsset = false);
	void updateLinkedAssetProperties();
	//Returns true if the input asset is directly linkable
	bool isLinkableAsset(QSharedPointer<Asset> asset);
	QSharedPointer<Property> getLinkedHostNameProperty();
	QSharedPointer<Property> getLinkedAssetHostIdProperty();
	QWeakPointer<Asset> linkedAsset_;

private slots:
	void linkedAssetPropertyEdited(Property* changedProp,QVariant newValue);
	void AssociateRootChildWasAdded(QSharedPointer<Asset> child);


};


}; //namespace Picto

#endif
