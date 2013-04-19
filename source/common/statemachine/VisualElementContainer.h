#ifndef _OutputElementContainer_H_
#define _OutputElementContainer_H_

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "../common.h"
#include "../statemachine/UIEnabled.h"
#include "../property/Property.h"

#include "../stimuli/OutputElement.h"
#include "StateMachineElement.h"

namespace Picto {

/*! \brief A container for storing Visual Elements
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API OutputElementContainer : public StateMachineElement
#else
class OutputElementContainer : public StateMachineElement
#endif
{
	Q_OBJECT
public:
	OutputElementContainer();
	virtual ~OutputElementContainer(){};

	void addOutputElements(QSharedPointer<OutputElementContainer> outputElementContainer);
	void addOutputElements(OutputElementContainer* outputElementContainer);
	void addOutputElement(QSharedPointer<OutputElement> outputElement);
	void addChildOutputElementContainer(QSharedPointer<OutputElementContainer> child);
	QList<QSharedPointer<OutputElement>> getOutputElementList();

	/*! \brief Initializes scripting for this and all child OutputElementContainers
	 * @param enableDebugging Enables debugging for this object.  Debugging will be
	 * triggered if there is a script exception or a "debugger;" line is input into the script.
	 * returns false if there was an initialization error.
	 */
	bool initOutputElementList();

	virtual QString assetType(){return "OutputElementContainer";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void OutputElementContainerWasReinitialized(){};

private:
	QList<QSharedPointer<OutputElement> > outputElements_;
	QMap<QString,QList<QSharedPointer<OutputElement>>> analysisOutputElementsByGuid_;
	QList<QSharedPointer<OutputElementContainer> > outputElementContainers_;
	bool isAnalysisElement_;
	bool elementListInitialized_;

private slots:
	//This is called if something about a OutputElement changed, so that the script
	//engines will need to be reinitialized before use.
	void deinitOutputElementList();
	void deinitOutputElementList(Property* prop,QVariant value);
	void addChildToElementLists(QSharedPointer<Asset> newChild);

private:
	friend class OutputElementContainer;
};

}; //namespace Picto

#endif
