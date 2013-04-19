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
	void addChildOutputElementContainer(QSharedPointer<OutputElementContainer> child);
	QList<QSharedPointer<OutputElement>> getOutputElementList();
	virtual QString assetType(){return "OutputElementContainer";};
	virtual void ClearAnalysisChildren(QUuid analysisId);
	virtual void enableRunMode(bool enable);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void addOutputElement(QSharedPointer<OutputElement> outputElement);
	QList<QSharedPointer<OutputElement> > outputElements_;
	QMap<QString,QList<QSharedPointer<OutputElement>>> analysisOutputElementsByGuid_;
	QList<QSharedPointer<OutputElementContainer> > outputElementContainers_;

private slots:
	void addChildToElementLists(QSharedPointer<Asset> newChild);

private:
	friend class OutputElementContainer;
};

}; //namespace Picto

#endif
