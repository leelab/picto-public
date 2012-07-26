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
	Q_OBJECT
public:
	StateMachineEditorData();
	virtual ~StateMachineEditorData(){};
	static QSharedPointer<Asset> Create();
	void setOpenedAsset(QSharedPointer<Asset> openedAsset);
	QSharedPointer<Asset> getOpenedAsset();
	void setOpenedAssetId(int openedAsset);
	int getOpenedAssetId();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QSharedPointer<Asset> findAssetWithID(QSharedPointer<Asset> root, int id);
};


}; //namespace Picto

#endif
