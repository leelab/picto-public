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
/*! \file */
//The \#defines below implements the AssociateElement for its child classes in a default way.
//This is really just a simple way of cleaning up the multiple inheritance necessary
//to all AssociateElements to inherit from design elements but still have common properties.
/*! \brief This \#define should be copied into the public section of anything inheriting AssociateElement in order to implement its pure virtual functions.
 *	\details Implements the AssociateElement class's pure virtual functions including:
 *		- getParentPath()	-	Returns the Path of the Design element to which this AssociateElement is linked (this is handled by a child AssociateHostLink).
 *		- getParentId()		-	Returns the Asset ID of the Design element to which this AssociateElement is linked (this is handled by a child AssociateHostLink).
 *		- linkToAsset(QSharedPointer<Asset> asset)		-	Updates and connects linked asset data fields for the input Asset (this is handled by a child AssociateHostLink)\
 *		- attachToLinkedAsset(bool tryLinkingById)		-	Attempts to attach this AssociateElement as an Associate Child to the current linked asset (set in
 *															linkToAsset()).  If tryLinkingById is true, we first attempt to link by looking up the Asset ID in the
 *															AssociateRootHost to which our parent AssociateRoot is attached.  If that doesn't work, or the input was
 *															false, we attempt to link by linked path.  Returned value is an AssociateElement::AttachmentResult.
 *		- getAssociateId()	-	Returns the Associate ID of this AssociateElement's parent AssociateRoot.
 *		- getInfoString()	-	Returns a string with this AssociateElement's name, type and linked path.
 *		- getLinkedAsset()	-	Returns a pointer to the Asset to which this AssociateElement is linked.
 *		- updateLinkPath(QString oldPrefix,QString newPrefix)	-	Changes the section at the beginning of the linked path which is input as oldPrefix to the input
 *																	newPrefix value.  This is useful when exporting a group of AnalysisElements from one element and importing 
 *																	them into another one.  In that case, we want to keep all of the imported elements relative paths the same
 *																	but change their parent path.
 *
 *	\note We don't just include this code in the AssociateElement because it would not complile unless the 
 *	AssociateElement class inherited from DataStore, and this would lead to AssociateElement children
 *	having diamond inheritance patterns (ie. inheriting from two classes that both inherit from the same class)
 *	which leads to lots of problems.
 */
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
												if(linkedAsset.staticCast<DataStore>()->AddAssociateChild(getAssociateId(),getParentPath(),identifier(),selfPtr()))	\
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
											};	\
											virtual void updateLinkPath(QString oldPrefix,QString newPrefix)	\
											{	\
												QSharedPointer<AssociateHostLink> lnk = getGeneratedChildren("HostLink").first().staticCast<AssociateHostLink>();	\
												return lnk->updateLinkPath(oldPrefix,newPrefix);	\
											};											

/*! \brief This \#define should be copied to the end of the constructor of anything inheriting AssociateElement.
 *	\details Adds a required child HostLink to the AssociateElement to store information about the Design element to
 *	which the AssociateElement should be linked.
 *	\note We don't just include this code in the AssociateElement itself because it would not complile unless the 
 *	AssociateElement class inherited from DataStore, and this would lead to AssociateElement children
 *	having diamond inheritance patterns (ie. inheriting from two classes that both inherit from the same class)
 *	which leads to lots of problems.
 */
#define EXP_LINK_FACTORY_CREATION AddDefinableObjectFactory("HostLink",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AssociateHostLink::Create))));

/*! \brief AssociateElements are the child elements of an AssociateRootHost.  They have the capability to link to a DesignElement to provide extra non-vital information
 *	about that element.
 *	\details CUrrent AssociateElements are used for Analysis and UI information in the Designer.  Neither of these need to be included for the experiment to run, but they do
 *	need to be addable into the Design to provided their additional functionality.  For this reason, they "link into" design elements.  They are deleted when design elements
 *	are deleted and their linked ids and paths are updated when the elements to which they are linked are edited; however, they can easily be unlinked from design elements
 *	wihtout affecting the integrity of the design.
 *	\note In order to properly inherit from AssociateElement, descendant classes need to copy the ASSOCIATE_ELEMENT_IMPLEMENTATION \#define into the public section of their
 *	.h file and copy the EXP_LINK_FACTORY_CREATION \#define to the end of their constructor.  We don't just include this code in the AssociateElement itself because it would 
 *	not complile unless the AssociateElement class inherited from DataStore.  AssociateElement children need to inherit from various DataStore classes themselves to be able
 *	to reuse DataStore sub-class code, and this would therefore lead to AssociateElement children having diamond inheritance patterns (ie. inheriting from two classes that 
 *	both inherit from the same class) which leads to lots of problems.
 *	\sa AssociateRoot, AssociateRootHost
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateElement
#else
class AssociateElement
#endif
{

public:
	/*! \brief Constructs a new AssociateElement.
	 *	\details Adds a required child HostLink to the AssociateElement to store information about the Design element to which the AssociateElement should be linked.
	 */
	AssociateElement(){};
	virtual ~AssociateElement(){};
	/*! \brief Returns the Path of the Design element to which this AssociateElement is linked (this is handled by a child AssociateHostLink).*/
	virtual QString getParentPath() = 0;
	/*! \brief Returns the Asset ID of the Design element to which this AssociateElement is linked (this is handled by a child AssociateHostLink).*/
	virtual int getParentId() = 0;
	/*! \brief Updates and connects linked asset data fields for the input Asset (this is handled by a child AssociateHostLink).*/
	virtual void linkToAsset(QSharedPointer<Asset> asset) = 0;
	/*! \brief The result of a call to attachToLinkedAsset.*/
	enum AttachmentResult { 
		SUCCESS_BY_LINK,	//!< This AssociateElement was succesfully linked to its linked Asset by looking up the Asset by Id.
		SUCCESS_BY_PATH,	//!< This AssociateElement was succesfully linked to its linked Asset by looking up the Asset bt path.
		FAIL				//!< This AssociateElement could not be linked to its linked Asset.
	};
	/*! \brief Attempts to attach this AssociateElement as an Associate Child to the current linked asset (set in linkToAsset()).
	 *	\details If tryLinkingById is true, we first attempt to link by looking up the Asset ID in the AssociateRootHost to which our parent AssociateRoot is attached.  
	 *	If that doesn't work, or the input was false, we attempt to link by linked path.  Returned value is an AssociateElement::AttachmentResult.
	 */
	virtual AttachmentResult attachToLinkedAsset(bool tryLinkingById) = 0;
	/*! \brief Returns the Associate ID of this AssociateElement's parent AssociateRoot.*/
	virtual QUuid getAssociateId() = 0;
	/*! \brief Returns a string with this AssociateElement's name, type and linked path.*/
	virtual QString getInfoString() = 0;
	/*! \brief Returns a pointer to the Asset to which this AssociateElement is linked.*/
	virtual QSharedPointer<Asset> getLinkedAsset() = 0;
	/*! \brief Changes the section at the beginning of the linked path which is input as oldPrefix to the input newPrefix value.
	 *	\details This is useful when exporting a group of AnalysisElements from one element and importing them into another one.  In that case, we want to keep all of 
	 *	the imported element's relative paths the same but change their parent path.
	 */
	virtual void updateLinkPath(QString oldPrefix,QString newPrefix) = 0;

private:
	QUuid AssociateRootId_;
};


}; //namespace Picto

#endif
