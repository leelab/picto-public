#ifndef _PICTODATA_H_
#define _PICTODATA_H_

#include "../experiment/experiment.h"
#include "../parameter/Analysis.h"
#include "../storage/DataStore.h"
#include <QUuid>

namespace Picto {

/*!	\brief The root node of the Picto design tree
 *	\details This DataStore is the root DataStore of the entire Picto Design.  Its children include
 *	Experiment objects, and any number of Analysis objects and UIData objects.  Whereas the 
 *	DesignRoot is a convenient package for moving designs around, the PictoData node is the real
 *	root of the design, included in the actual serialization/deserialization procedure.
 *	
 *	\sa DesignRoot
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Sets the input name to this object.
	 *	\details If setName is not called, the object's default name is "Untitled"
	 */
	virtual void setName(QString newName){propertyContainer_->setPropertyValue("Name",newName);};
	/*! \brief Returns a string containing the name of this object
	*/
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
