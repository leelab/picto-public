#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include <QObject>

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

/*!	\brief A transition between two StateMachineElements
 *
 *	A transition links a specific Result value from a specific StateMachineElement
 *	to another StateMachineElement.  Since no two StateMachineElements on the same level are allowed
 *	to have the same name, all of the connected Assets can be referenced by name.  We serialize AssetId references
 *	and keep pointers to the Assets as well though in order to avoid problems when designers accidentally name two things with the same
 *	name (this can also happen briefly if one StateMachineElement's name is equal to anothers with additional letters
 *	on the end).
 *
 *	Transitions are mainly just containers for Source, SourceResult and Destination data; however they also include
 *	functionality to automatically delete themselves if one of the Assets to which they are connected is deleted.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Transition : public DataStore
#else
class Transition  : public DataStore
#endif
{
	Q_OBJECT
public:
	Transition();
	virtual ~Transition(){};
	Transition(QSharedPointer<Asset> source, QSharedPointer<Asset> sourceResult, QSharedPointer<Asset> destination);
	Transition(QString result);
	Transition(QSharedPointer<Asset> source,QString sourceResult);
	//Transition(QString source, QString sourceResult, QString destination);
	static QSharedPointer<Asset> Create();

	void setSource(QSharedPointer<Asset> source);
	void setSourceResult(QSharedPointer<Asset> sourceResult);
	void setDestination(QSharedPointer<Asset> destination);

	QString getSource();
	QString getSourceResult();
	QString getDestination();

	int getSourceId();
	int getSourceResultId();
	int getDestinationId();
	
	QSharedPointer<Asset>  getSourceAsset();
	QSharedPointer<Asset>  getSourceResultAsset();
	QSharedPointer<Asset>  getDestinationAsset();
	
	//DataStore functions
	virtual QString friendlyTypeName(){return "Transition";};

protected:
	virtual QString defaultTagName(){return "Transition";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Asset> sourceAsset_;
	QSharedPointer<Asset> sourceResultAsset_;
	QSharedPointer<Asset> destinationAsset_;
private slots:
	void setValuesFromAssets();
	void linkedAssetDeleted();

};


}; //namespace Picto

#endif
