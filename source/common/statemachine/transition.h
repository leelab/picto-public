/*!	\Brief A transition between two StateMachineElements
 *
 *	A transition links a specific Result value from a specific StateMachineElement
 *	to another StateMachineElement.  Everything is referenced by by name.
 */
#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include <QObject>

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API Transition : public QObject, public DataStore
#else
class Transition  : public QObject, public DataStore
#endif
{
	Q_OBJECT

public:
	Transition();
	Transition(QString source, QString sourceResult, QString destination);

	void setSource(QString source) { source_ = source; };
	void setSourceResult(QString sourceResult) { sourceResult_ = sourceResult; };
	void setDestination(QString destination) { destination_ = destination; };

	QString getSource() { return source_; };
	QString getSourceResult() { return sourceResult_; };
	QString getDestination() { return destination_; };
	
	//DataStore functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString source_;
	QString sourceResult_;
	QString destination_;

};


}; //namespace Picto

#endif