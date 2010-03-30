/*!	\brief Contains info about a single session
 *
 *	The Server will likely be handling multiple concurrent sessions,
 *	so we need to be able to keep track of them.  The SessionInfo
 *	object is used by the SessionManager to keep track of everything.
 *
 *	Initially this will be more of a struct than an object (although I am
 *	keeping the member variables private).  I am creating it as a seperate object
 *	since it seems possible that we may need to add functionality at a 
 *	later date.
 *
 *	Since we don't want random objects changing the values of the contained variables
 *	I am not providing setter functions.  Instead, this is a fried of the 
 *	SessionManager class, so it will be the only object able to set these
 *	values.  As a result, it is perfectly safe to pass these objects around
 *	(ideally we should pass around pointers to them).
 */

#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include "../datacollection/alignmenttool.h"
#include "../datacollection/neuraldatacollector.h"
#include "../network/serverthread.h"

#include <QObject>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QUuid>
#include <QTimer>

class SessionInfo : public QObject
{
	Q_OBJECT

public:
	SessionInfo();

	QSqlDatabase sessionDb() { return sessionDb_; };
	QUuid uuid() { return uuid_; };
	QSharedPointer<AlignmentTool> alignmentTool() { return alignmentTool_; };
	QSharedPointer<NeuralDataCollector> neuralDataCollector() { return ndc_; };

	QString pendingDirective();
	void setPendingDirective(QString directive) { pendingDirective_ = directive; };

	void setTimeoutInterval(int timoutMs);
	void resetTimeout();

	friend class SessionManager;

signals:
	void timeout();

private:
	QUuid uuid_;
	QSqlDatabase sessionDb_;
	QSharedPointer<AlignmentTool> alignmentTool_;
	QSharedPointer<NeuralDataCollector> ndc_;
	QTimer timeoutTimer_;
	QString pendingDirective_;
};

#endif

