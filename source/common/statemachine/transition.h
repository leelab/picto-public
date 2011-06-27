#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include <QObject>

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

/*!	\Brief A transition between two StateMachineElements
 *
 *	A transition links a specific Result value from a specific StateMachineElement
 *	to another StateMachineElement.  Everything is referenced by by name.
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
	Transition(QString source, QString sourceResult, QString destination);
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new Transition());};

	void setSource(QString source);
	void setSourceResult(QString sourceResult);
	void setDestination(QString destination);

	void setSource(QSharedPointer<Asset> source);
	void setSourceResult(QSharedPointer<Asset> sourceResult);
	void setDestination(QSharedPointer<Asset> destination);

	QString getSource();
	QString getSourceResult();
	QString getDestination();
	
	//DataStore functions
	//virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual QString assetType(){return "Transition";};

protected:
	virtual QString defaultTagName(){return "Transition";};
	virtual void postSerialize();
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
