#ifndef _AssociateRoot_H_
#define _AssociateRoot_H_

#include "../common.h"
#include "../StateMachine/ScriptableContainer.h"

namespace Picto {

/*! \brief An AssociateRoot variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssociateRoot : public ScriptableContainer
#else
class AssociateRoot : public ScriptableContainer
#endif
{
	Q_OBJECT

public:
	AssociateRoot();
	virtual ~AssociateRoot(){};
	virtual QString assetType(){return "AssociateRoot";};
	QUuid getAssociateId(){return propertyContainer_->getPropertyValue("AssociateId").toUuid();};
	QUuid getLinkedHostId(){return propertyContainer_->getPropertyValue("LinkedHostId").toUuid();};
	QString getLinkedHostName(){return propertyContainer_->getPropertyValue("LinkedHostName").toString();};
	bool LinkToAsset(QSharedPointer<Asset> asset, QString& feedback);
	QSharedPointer<Asset> getLinkedAsset(){return linkedAsset_;};
	//If the PictoData root (this element's parent) contains the asset that this AssociateRoot is supposed to 
	//link to then this function returns it, otherwise it returns an empty shared pointer.
	QSharedPointer<Asset> getLinkableAsset();

protected:
	virtual QString defaultTagName(){return "AssociateRoot";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	void setLinkedAsset(QSharedPointer<Asset> asset);
	void updateLinkedAssetProperties();
	//Returns true if the input asset is directly linkable
	bool isLinkableAsset(QSharedPointer<Asset> asset);
	QSharedPointer<Property> getLinkedHostNameProperty();
	QSharedPointer<Property> getLinkedAssetHostIdProperty();
	QWeakPointer<Asset> linkedAsset_;
	QSharedPointer<AssetFactory> variableFactory_;
	QSharedPointer<AssetFactory> dataSourceFactory_;
	QSharedPointer<AssetFactory> outputFactory_;
	QSharedPointer<AssetFactory> functionFactory_;
	QSharedPointer<AssetFactory> scriptFactory_;

private slots:
	void linkedAssetPropertyEdited(Property* changedProp,QVariant newValue);
	void AssociateRootChildWasAdded(QSharedPointer<Asset> child);


};


}; //namespace Picto

#endif
