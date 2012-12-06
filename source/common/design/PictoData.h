#ifndef _PICTODATA_H_
#define _PICTODATA_H_

#include "../experiment/experiment.h"
#include "../storage/DataStore.h"
#include <QUuid>

namespace Picto {

/*!	\brief Holds all Picto data that was serialized in including experiment and ui data
 *	
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PictoData : public DataStore
#else
class PictoData : public DataStore
#endif
{
public:
	static QSharedPointer<Asset> Create();
	virtual ~PictoData(){};
	virtual QString assetType(){return "PictoData";};
	virtual QString getName(){return propertyContainer_->getPropertyValue("Name").toString();};
	QSharedPointer<Experiment> getExperiment();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	PictoData();
};


}; //namespace Picto

#endif
