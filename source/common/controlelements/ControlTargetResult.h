#ifndef _ControlTargetResult_H_
#define _ControlTargetResult_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "../statemachine/LogicResult.h"
#include "controltarget.h"
#include <QRect>

namespace Picto {

/*!	\brief A ControlTargetResult is used by ChoiceController for defining the ControlTarget elements that the subject may choose from.
 *	\details Each ControlTargetResult allows the designer to select a single ControlTarget.  If that ControlTarget is selected,
 *	that ControlTargetResult will be set as the ChoiceController element's result.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlTargetResult : public LogicResult
#else
class ControlTargetResult : public LogicResult
#endif
{
	Q_OBJECT
public:
	ControlTargetResult();
	virtual ~ControlTargetResult(){};
	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Control Target Result";};
	bool contains(int x, int y);
	void setActive(bool active);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void scriptableContainerWasReinitialized();
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
private:
	QWeakPointer<ControlTarget> controlTarget_;
};


}; //namespace Picto

#endif
