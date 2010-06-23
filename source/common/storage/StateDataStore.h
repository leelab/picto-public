/*!	\brief Stores behavioral data as x,y,t triplets
 *
 *	The state data store is used for storing the current state of the engine's 
 *	state machine.  These are generated when there is some sort of change in the
 *	state machine.
 *
 *	The XML format is:
 *	<StateDataStore timestamp = 123.456 statemachine="state machine name">
 *		<Transition>
 *			<Source>TestState</Source>
 *			<SourceResult>Success</SourceResult>
 *			<Destination>Done</Destination>
 *		</Transition>
 *	</StateDataStore>
 */

#ifndef _STATEDATASTORE_H_
#define _STATEDATASTORE_H_

#include "../common.h"
#include "DataStore.h"
#include "../statemachine/transition.h"

#include <QString>

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API StateDataStore : public DataStore
#else
class StateDataStore : public DataStore
#endif
{
public:
	StateDataStore();

	void setTransition(QSharedPointer<Transition> transition, double timestamp, QString stateMachineName);
	void setTransition(QString source, QString sourceResult, QString destination, double timestamp, QString stateMachineName);
	void clearTransition() { transition_ = QSharedPointer<Transition>(); };

	QString getMachineName() { return machineName_; };
	double getTime() { return timestamp_; };
	QString getSource() { return transition_->getSource(); };
	QString getSourceResult() { return transition_->getSourceResult(); };
	QString getDestination() { return transition_->getDestination(); };

	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QSharedPointer<Transition> transition_;
	double timestamp_;
	QString machineName_;
};


}; //namespace Picto

#endif
