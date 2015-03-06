#ifndef _RESULTCONTAINER_H
#define _RESULTCONTAINER_H
#include <QString>
#include <QSharedPointer>
#include <QMap>

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"
#include "Transition.h"
#include "Result.h"
namespace Picto {

/*!	\brief A base class for elements that may contain Result elements.
 *	\details Since Results are a fundamental part of the Picto StateMachine, ResultContainers
 *	are very important.  The ResultContainer class is a base class for elements like StateMachine, 
 *	State, SwitchElement, etc.  This class contains functionality for adding Results, setting up 
 *	required and optional Results and different types of Results, as well as accessing the latest 
 *	triggered result name from a script during execution.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ResultContainer : public ScriptableContainer
#else
class ResultContainer : public ScriptableContainer
#endif
{
	Q_OBJECT
public:
	ResultContainer();
	virtual ~ResultContainer(){};

	virtual void enteredScope();
	QStringList getResultList();
	QSharedPointer<Result> getResult(QString name);
	virtual bool hasEditableDescendants();	
	
public slots:
	QString getLatestResult();

protected:
	void addRequiredResult(QString resultName);
	void setMaxOptionalResults(int max, QString type = "");
	void defineResultFactoryType(QString type,QSharedPointer<AssetFactory> resultFactory);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void setLatestResult(QString latestResult);
	QMap<QString, QSharedPointer<Result> > results_;

	friend class SlaveExperimentDriver;
	friend class State;

private:
	QSharedPointer<AssetFactory> resultFactory_;
	QMap<QString,QSharedPointer<AssetFactory>> resultFactoryByType_;
	QMap<QString,int> maxOptionalResults_;
	QMultiMap<QString,QSharedPointer<Result> > requiredResults_;
	QString latestResult_;
//private slots:
//	void updateResultsFromChildren();

};


}; //namespace Picto

#endif
