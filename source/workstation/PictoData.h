#ifndef _PICTODATA_H_
#define _PICTODATA_H_

#include "../common/experiment/experiment.h"
#include "../common/storage/DataStore.h"
#include "statemachineeditor/statemachineeditordata.h"
#include <QUuid>

namespace Picto {

/*!	\brief Holds all Picto data that was serialized in including experiment and ui data
 *	
 */

class PictoData : public DataStore
{
public:
	static QSharedPointer<Asset> Create();
	virtual QString assetType(){return "PictoData";};
	QSharedPointer<Experiment> getExperiment();
	QSharedPointer<StateMachineEditorData> getStateMachineEditorData();

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	PictoData();
};


}; //namespace Picto

#endif
