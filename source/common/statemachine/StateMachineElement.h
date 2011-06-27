#ifndef _STATEMACHINEELEMENT_H_
#define _STATEMACHINEELEMENT_H_

#include "../common.h"

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>
#include <QStringList>

#include "../parameter/ParameterContainer.h"
#include "../parameter/Parameter.h"
#include "../property/PropertyContainer.h"
#include "../storage/DataStore.h"
#include "../engine/PictoEngine.h"
#include "../network/CommandChannel.h"
#include "../protocol/ProtocolResponse.h"
#include "../statemachine/result.h"
#include "../parameter/ParameterContainer.h"

namespace Picto {

class Result;
//class Engine::PictoEngine;

/*!	\brief A StateMachineElement is a piece that can be used in a state machine
 *	
 *	StateMachineElements include FlowElements, Results, States, and StateMchines
 *	themselves. Not all StateMachineElements need all of the functionality provided.
 *
 *	Calling run() on a state machine element returns a result.
 *
 *	StateMchineElements are connected by transitions objects.  A transition
 *	connects a specific result from a specific StateMachineElement to antother
 *	specific StateMachineElement.
 *
 *	Parameters are local+global in scope, so a StateMachineElement has access to 
 *	its own parameters as well as the parameters from above it in the hierarchy.
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachineElement : public ParameterContainer
#else
class StateMachineElement : public ResultContainer
#endif
{
	Q_OBJECT

public:
	StateMachineElement();

	//All StateMachineElements must implement a run function that returns a string
	//The returned string should correspond to a result contained by the element
	virtual QString run(QSharedPointer<Engine::PictoEngine> engine) = 0;
	virtual QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine) = 0;
	static void resetSlaveElements(double time = 0.0) { lastTransitionTime_ = time; };
	
	//bool addResult(QSharedPointer<Result> result);
	//QStringList getResultList();
	////QSharedPointer<Result> getResult(QString name);
	//void addParameters(QSharedPointer<ParameterContainer> parameters);
	//void addParameters(ParameterContainer &parameters);

	QString type();
	virtual QString assetType(){return "StateMachineElement";};

	virtual QPoint getDisplayLayoutPosition();

	//virtual bool initScripting(QScriptEngine &qsEngine) { Q_UNUSED(qsEngine); return true; }

	//These will need to be implemented in all subclasses for the GUI
	/*virtual QRect getDisplayBoundingRect() = 0;
	virtual void draw() = 0;*/

	//DataStore functions
	//virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	//virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

	//bool serializeResults(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeResults(QSharedPointer<QXmlStreamReader> xmlStreamReader);


protected:
	virtual QString defaultTagName(){return "StateMachineElement";};
	QString getMasterStateResult(QSharedPointer<Engine::PictoEngine> engine);
	void processStatusDirective(QSharedPointer<Engine::PictoEngine> engine, QSharedPointer<ProtocolResponse> dataResponse);
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	//ParameterContainer parameterContainer_;
	//QMap<QString,QSharedPointer<Result>> results_;

	QPoint layoutPosition_;

	static double lastTransitionTime_;
};


}; //namespace Picto

#endif
