#include <QStringList>
#include "ProtocolResponseHandler.h"
#include "../memleakdetect.h"
using namespace Picto;

ProtocolResponseHandler::ProtocolResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
statusManager_(statusManager),
processingResponse_(false)
{
	Q_ASSERT(!statusManager_.isNull());
}


void ProtocolResponseHandler::acceptResponse(QSharedPointer<ProtocolResponse> response)
{
	//If this response was received while processing the same response, somebody did something wrong.
	if(processingResponse_)
	{
		qDebug("ProtocolResponseHandler::acceptResponse: The component received a directive of the same type that it was currently processing.\n Response handlers only support one level of recursion for the same response type.");
		return;
	}
		
	//Parse Response
	QString directive = response->getDecodedContent();
	int directiveEnd = directive.indexOf(' ');
	int altDirectiveEnd = directive.indexOf('\n');
	if((altDirectiveEnd > 0) && (altDirectiveEnd < directiveEnd))directiveEnd = altDirectiveEnd;
	QString statusDirective = directive.left(directiveEnd);
	directive.remove(0,directiveEnd+1);
	processingResponse_ = true;
	Q_ASSERT(!statusManager_.isNull());
	if(processResponse(directive))
		statusManager_.toStrongRef()->setStatus(statusDirective);
	#ifdef ENABLE_MEMORY_LEAK_DETECTION
	//This Enables Memory Leak Detection
	if(method() == "START")
		_CrtDumpMemoryLeaks();
	/////////////////////////////////////////////////////////////////
	#endif
	processingResponse_ = false;
}

