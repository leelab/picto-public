#ifndef _TESTCOMMANDHANDLER_H_
#define _TESTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*!	\brief A command handler used for internal testing.
 *
 *	I created this command handler when I was first playing with the server.  It was 
 *	an experiment to see if I could create my own command handler.  It has since been 
 *	used as a testbed to see how different commands might work.  Feel free to play with
 *	it or throw it away.
 */
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
