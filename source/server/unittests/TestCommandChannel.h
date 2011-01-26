#include <QtTest/QtTest>
#include "../../common/globals.h"
#include "../../common/network/CommandChannel.h"

#include <QHostAddress>

/*! \brief Contains unit tests for CommandChannelObject
 *
 *	The TestCommandChannel object contains the unti tests for the command channel object.  The
 *	tests assume that there is a Picto server running, and rely on the server's ability to
 *	respond to certain types of commands.
 */
class TestCommandChannel: public QObject
{
    Q_OBJECT


public:
	TestCommandChannel(QHostAddress _serverAddress,quint16 _port);

private:

	QHostAddress serverAddress_;
	quint16 port_;

	Picto::CommandChannel *channel_;

public slots:
	

private slots:

	////////////////////////////
	// I rebuilt the command channel so that it only used the polling mode.  As a result
	// many of these tests will no longer work.
	/////////////////////////////
	void initTestCase();

	//NOTE: The test names may be somewhat confusing. If it includes "command"
	//then the test involves *sending* commands (consumer mode).  If it 
	//includes "response", then the test involves *sending* responses
	//(producer mode).
	void polledSingleCommand();
	void polledMultipleCommands();
	//void polledServerConnectionDropped();

	//void eventDrivenSingleCommand();
	//void eventDrivenMultipleCommands();
	//void eventDrivenServerConnectionDropped();

	//void streamingResponse();

	void polledResponses_data();
	//void polledResponses();
	//void eventDrivenResponses_data();
	//void eventDrivenResponses();

	void cleanupTestCase();
};
