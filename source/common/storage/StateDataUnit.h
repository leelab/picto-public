#ifndef _STATEDATAUNIT_H_
#define _STATEDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"
#include "../statemachine/transition.h"

#include <QString>

namespace Picto {

/*!	\brief Stores state transitions.
 *
 *	The state data store is used for storing the current state of the engine's 
 *	state machine.  These are generated when there is some sort of change in the
 *	state machine.
 *
 *	The XML format is:
 *	<StateDataUnit timestamp = 123.456 statemachinepath="state machine name">
 *		<Transition>
 *			<Source>TestState</Source>
 *			<SourceResult>Success</SourceResult>
 *			<Destination>Done</Destination>
 *		</Transition>
 *	</StateDataUnit>
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API StateDataUnit : public DataUnit
#else
class StateDataUnit : public DataUnit
#endif
{
public:
	StateDataUnit();

	void setTransition(QSharedPointer<Transition> transition, double timestamp, QString stateMachinePath);
	void setTransition(QString source, QString sourceResult, QString destination, double timestamp, QString stateMachinePath);
	void clearTransition() { transition_ = QSharedPointer<Transition>(); };

	QString getMachinePath() { return machinePath_; };
	double getTime() { return timestamp_; };
	QString getSource() { return transition_->getSource(); };
	QString getSourceResult() { return transition_->getSourceResult(); };
	QString getDestination() { return transition_->getDestination(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Transition> transition_;
	double timestamp_;
	QString machinePath_;
};


}; //namespace Picto

#endif
