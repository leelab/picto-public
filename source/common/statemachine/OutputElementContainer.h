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

/*! \brief A base class for elements that hold OutputElement objects.
 *
 *	\details This class contains the AssetFactory objects for creating OutputElements and manages
 *	their scoping issues so that descendant classes can easily access all in-scope OutputElements
 *	from a single list.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	virtual bool hasEditableDescendants(){return true;};

	void addOutputElements(QSharedPointer<OutputElementContainer> outputElementContainer);
	void addOutputElements(OutputElementContainer* outputElementContainer);
	void addChildOutputElementContainer(QSharedPointer<OutputElementContainer> child);
	QList<QSharedPointer<OutputElement>> getOutputElementList();
	virtual QString friendlyTypeName(){return "Output Element Container";};
	virtual void ClearAssociateChildren(QUuid associateId);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QSharedPointer<AssetFactory> visualElementFactory_;
	QSharedPointer<AssetFactory> audioElementFactory_;
	QSharedPointer<AssetFactory> outputSignalFactory_;
	QSharedPointer<AssetFactory> scriptFunctionFactory_;

private:
	void addOutputElement(QSharedPointer<OutputElement> outputElement);
	QList<QSharedPointer<OutputElement> > outputElements_;
	QList<QSharedPointer<OutputElementContainer> > outputElementContainers_;

private slots:
	void addChildToElementLists(QSharedPointer<Asset> newChild);

private:
	friend class OutputElementContainer;
};

}; //namespace Picto

#endif
