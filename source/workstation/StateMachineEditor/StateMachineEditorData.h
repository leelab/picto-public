#ifndef _STATEMACHINEEDITORDATA_H_
#define _STATEMACHINEEDITORDATA_H_

#include "../common/storage/DataStore.h"
#include <QUuid>

namespace Picto {

/*!	\brief A StateMachineEditorData is the exit point from a StateMachineElement or ControlElement
 *	
 */

class StateMachineEditorData : public DataStore
{
public:
	StateMachineEditorData();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new StateMachineEditorData());};
	void setOpenedAsset(QSharedPointer<Asset> openedAsset);
	QSharedPointer<Asset> getOpenedAsset();
	void setOpenedAssetId(QUuid openedAsset);
	QUuid getOpenedAssetId();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QSharedPointer<Asset> findAssetWithID(QSharedPointer<Asset> root, QUuid id);
};


}; //namespace Picto

#endif
