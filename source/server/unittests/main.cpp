#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QtTest>
#include <QProcess>
#include <QTextStream>

#include "TestBasicProtocolCommands.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

	QHostAddress serverAddress;

	if(argc>1)
	{
		QString runTestsTest(argv[1]);

		if(runTestsTest == "RUNTESTS")
		{
			if(argc>2)
			{
				serverAddress.setAddress(argv[2]);
			}
			else
			{
				serverAddress.setAddress("127.0.0.1");
			}

			TestBasicProtocolCommands basicProtocolCommandTestObject(serverAddress);

			QStringList testOptionList;

			//We first need to add the executable image name so that it can properly be added
			//as the first argument.  Otherwise, our other arguments will be ignored.
			testOptionList += argv[0];
			//testOptionList += "-xml";

			return QTest::qExec(&basicProtocolCommandTestObject, testOptionList);
		}
	}

	QString program(argv[0]);
	QStringList arguments;

	arguments += argv[0];
	arguments += "RUNTESTS";
	
	if(argc>1)
	{
		arguments += argv[1];
	}

	QProcess testSubProcess;

	testSubProcess.start(program,arguments);
	/*! \todo this should actually enter an event loop, and provide feedback while running the tests */
	testSubProcess.waitForFinished(-1);
	QByteArray results = testSubProcess.readAllStandardOutput();

	//Process results here
	QTextStream outputStream(stdout);
	outputStream << results;

	return 0;
}
