#ifndef _TESTCOMMANDHANDLER_H_
#define _TESTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct TestCommandHandler : Picto::ProtocolCommandHandler
{
public:
	TestCommandHandler();

	QString method() { return QString("TEST"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
private:
	void doAlignTest();
};

#endif
