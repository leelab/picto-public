#ifndef _OutputElementContainer_H_
#define _OutputElementContainer_H_

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "../common.h"
#include "UIEnabled.h"
#include "../property/Property.h"

#include "../stimuli/OutputElement.h"
#include "StateMachineElementContainer.h"

namespace Picto {

/*! \brief A base class for elements that hold OutputElement objects.
 *
 *	\details This class contains the AssetFactory objects for creating OutputElements and manages
 *	their scoping issues so that descendant classes can easily access all in-scope OutputElements
 *	from a single list.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API OutputElementContainer : public StateMachineElementContainer
#else
class OutputElementContainer : public StateMachineElementContainer
#endif
{
	Q_OBJECT
public:
	OutputElementContainer();
	virtual ~OutputElementContainer(){};

	virtual bool hasEditableDescendants(){return true;};

	virtual QString friendlyTypeName(){return "Output Element Container";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QSharedPointer<AssetFactory> visualElementFactory_;
	QSharedPointer<AssetFactory> audioElementFactory_;
	QSharedPointer<AssetFactory> outputSignalFactory_;
	QSharedPointer<AssetFactory> inputSignalFactory_;
	QSharedPointer<AssetFactory> scriptFunctionFactory_;

private:
	friend class OutputElementContainer;
};

}; //namespace Picto

#endif
