#ifndef _DATA_VIEW_ELEMENT_CONTAINER_H_
#define _DATA_VIEW_ELEMENT_CONTAINER_H_

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "../common.h"
#include "../statemachine/UIEnabled.h"
#include "../property/Property.h"

#include "DataViewElement.h"
#include "../statemachine/StateMachineElementContainer.h"

namespace Picto {

/*! \brief A base class for elements that hold DataViewElement objects.
 *
 *	\details This class contains the AssetFactory objects for creating DataViewElements and manages
 *	their scoping issues so that descendant classes can easily access all in-scope DataViewElements
 *	from a single list.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API DataViewElementContainer : public StateMachineElementContainer
#else
class DataViewElementContainer : public StateMachineElementContainer
#endif
{
	Q_OBJECT
public:
	DataViewElementContainer();
	virtual ~DataViewElementContainer(){};

	virtual bool hasEditableDescendants(){ return true; };

	virtual QString friendlyTypeName(){ return "Data View Element Container"; };

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QSharedPointer<AssetFactory> dataViewElementFactory_;

private:
	friend class DataViewElementContainer;
};

}; //namespace Picto

#endif
