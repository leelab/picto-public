#ifndef _AssociateHostLink_H_
#define _AssociateHostLink_H_

#include <QSharedPointer>
#include "../common.h"
#include "../storage/asset.h"
#include "../storage/DataStore.h"

namespace Picto {

/*! \brief This is a simple object used to store data about the "host Asset" to which an AssociateElement links.
 *	\details The object includes two Properties, ParentPath and ParentId that store data about the Asset to which
 *	the AssociateElement links.  Since this object is a required child of an AssociateElement, the Property values
 *	will be saved with the AssociateElement so that a record will be maintained of the Asset to which the AssociateElement
 *	should link.
 *	
 *	Other functionality is included here too.  Once an Asset is linked using the linkToAsset() function, its
 *	signals are connected so that changes in the linked Asset name or id will automatically propogate to this
 *	object's Property values.  updateLinkPath() functionality is also provided for the purpose of AssociateElement
 *	export/import.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

	/*! \brief Returns the path of the linked Asset.
	 *	\details Elements in the path are separated by a "::" symbol.
	 */
	QString getParentPath(){return propertyContainer_->getPropertyValue("ParentPath").toString();};
	/*! \brief Returns the Asset Id of the linked Asset.
	 */
	int getParentId(){return propertyContainer_->getPropertyValue("ParentId").toInt();};
	/*! \brief Returns a shared Asset pointer to the linked Asset.
	*/
	QSharedPointer<Asset> getLinkedAsset(){return linkedAsset_.toStrongRef();};
	void linkToAsset(QSharedPointer<Asset> asset);
	void updateLinkPath(QString oldPrefix,QString newPrefix);

signals:
	/*! \brief Emitted when this object links to a new Asset.  The new Asset is passed in the input.*/
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
