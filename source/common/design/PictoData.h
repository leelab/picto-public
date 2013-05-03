#ifndef _PICTODATA_H_
#define _PICTODATA_H_

#include "../experiment/experiment.h"
#include "../parameter/Analysis.h"
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
	virtual QString friendlyTypeName(){return "Picto Data";};
	virtual QString getName(){return propertyContainer_->getPropertyValue("Name").toString();};
	QSharedPointer<Experiment> getExperiment();
	QList<QSharedPointer<Analysis>> getAnalyses();

protected:
	virtual QString defaultTagName(){return "PictoData";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	PictoData();
};


}; //namespace Picto

#endif
