#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include <QObject>

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

/*!	\Brief A transition between two StateMachineElements
 *
 *	A transition links a specific Result value from a specific StateMachineElement
 *	to another StateMachineElement.  Everything is referenced by by name.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API Transition : public DataStore
#else
class Transition  : public DataStore
#endif
{
	Q_OBJECT
public:
	Transition();
	Transition(QString source, QString sourceResult, QString destination);
	static QSharedPointer<Serializable> Create(){return QSharedPointer<Serializable>(new Transition());};

	void setSource(QString source) { source_ = source; };
	void setSourceResult(QString sourceResult) { sourceResult_ = sourceResult; };
	void setDestination(QString destination) { destination_ = destination; };

	QString getSource() { return source_; };
	QString getSourceResult() { return sourceResult_; };
	QString getDestination() { return destination_; };
	
	//DataStore functions
	//virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString source_;
	QString sourceResult_;
	QString destination_;

};


}; //namespace Picto

#endif
