#include "SessionInfo.h"

SessionInfo::SessionInfo()
{
	connect(&timeoutTimer_, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

//! Sets the timeout interval for the session
void SessionInfo::setTimeoutInterval(int timoutMs)
{
	timeoutTimer_.setInterval(timoutMs);
}

//! Resets timout timer.  This should be called anytime there is activity on the session
void SessionInfo::resetTimeout()
{
	timeoutTimer_.start();
}

//! Clears the session's pending directive, and returns it
QString SessionInfo::pendingDirective()
{
	QString temp = pendingDirective_;
	pendingDirective_ = "";
	return temp;
}


