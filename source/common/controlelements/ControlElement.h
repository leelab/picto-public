#ifndef _CONTROLELEMENT_H_
#define _CONTROLELEMENT_H_

#include <QObject>

#include "../common.h"

#include "../property/PropertyContainer.h"
#include "../storage/DataStore.h"
#include "../engine/PictoEngine.h"
#include "../statemachine/result.h"
#include "../statemachine/resultcontainer.h"


namespace Picto {

/*! \brief Triggers a result according to a logic test that runs each frame, thereby ending the execution of its parent State.
 *
 *	All Picto State elements must include a Control Element of some sort.  The control
 *	element determines when the State ends.  During the State element's rendering loop,
 *	the isDone() function is called every frame until it returns true.  Then the
 *	getResult() function is called to determine the result of the ControlElement's
 *	execution(e.g. "Broke fixation", "Selected target", etc)
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlElement : public ResultContainer
#else
class ControlElement : public ResultContainer
#endif
{
	Q_OBJECT

public:
	ControlElement();
	virtual ~ControlElement(){};

	/*! \brief isDone will return true if the ControlElement's logic determines that some condition has been met.*/
	virtual bool isDone(QSharedPointer<Engine::PictoEngine> engine) {return true;};

	using ResultContainer::getResult;
	/*! \brief Returns the latest result after a ControlElement isDone().
	 *	If the ControlElement is not done, this returns an empty result pointer.
	 */
	virtual QSharedPointer<Result> getResult() {return QSharedPointer<Result>();};

	/*! \brief Initializes the ControlElement.
	 *	\details Should be called when a State is first entered.
	 *	\note activateTargets() should be called by this function if ControlTarget elements will be used.  This will cause their
	 *	bounding geometries to be highlighted in the operator view until they are deactivated.
	 *	\sa ActivateTargets()
	 */
	virtual void start(QSharedPointer<Engine::PictoEngine> engine){};
	/*! \brief Deinitializes the ControlElement
	 *	\details Should be called when a state is about to exit.
	 *	\note deactivateTargets() should be called by this function if ControlTarget elements will be used.  This will cause their
	 *	bounding geometries to stop being highlighted.
	 *	\sa deactivateTargets()
	 */
	virtual void stop(QSharedPointer<Engine::PictoEngine> engine){};

	/*! \brief Activates all Control Targets managed by this Control Element.
	 *	\details This causes their bounding geometries to be highlighted in the operator view.
	 *	\sa ControlTarget
	 */
	virtual void activateTargets(){};
	/*! \brief Deactivates all Control Targets managed by this Control Element.
	 *	\details This causes their bounding geometries to stop being highlighted in the operator view.
	 *	\sa ControlTarget
	 */
	virtual void deactivateTargets(){};

	virtual QString getUITemplate(){return "ControlElement";};
	virtual QString friendlyTypeName(){return "Control Element";};
	virtual QString getUIGroup(){return "State Machine Elements";};

protected:
	virtual QString defaultTagName(){return "Control Element";};
	//bool addResult(QSharedPointer<Result> result);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	bool isDone_;

};


}; //namespace Picto

#endif
