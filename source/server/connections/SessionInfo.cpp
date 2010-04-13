#include "SessionInfo.h"

SessionInfo::SessionInfo()
{
	activity_=true;
}

//! Returns the next pending directive
QString SessionInfo::pendingDirective()
{
	if(pendingDirectives_.isEmpty())
		return QString();
	else
		return pendingDirectives_.takeFirst();
}


