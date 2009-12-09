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
	TestCommandChannel(QHostAddress _serverAddress);

private:

	QHostAddress serverAddress_;
	quint16 port_;

	Picto::CommandChannel *channel_;

public slots:
	

private slots:
	void initTestCase();

	void polledSingleCommand();
	void polledMultipleCommands();
	void polledServerConnectionDropped();

	void eventDrivenSingleCommand();
	void eventDrivenMultipleCommands();
	void eventDrivenServerConnectionDropped();

	void cleanupTestCase();
};
