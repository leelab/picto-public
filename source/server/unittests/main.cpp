/*!	\file server/unittests/main.cpp
 *
 *	This is the main unit test program for PictoServer.
 */

#include <QLocale>
#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QtTest>
#include <QProcess>
#include <QTextStream>

#include "../../common/common.h"
#include "../../common/globals.h"
#include "../../common/network/serverdiscoverer.h"
#include "TestBasicProtocolCommands.h"
#include "TestCommandChannel.h"
#include "PictoServerTest.h"

int main(int argc, char *argv[])
{
	int result=0;
	// SETUP
    QCoreApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	Picto::InitializeLib(&app,localeLanguageCode);

	//Setup testOptionList.  We first need to add the executable image name so that it can properly be added
	//as the first argument.  Otherwise, our other arguments will be ignored.
	QStringList testOptionList;
	testOptionList += argv[0];
	//testOptionList += "-xml";

	// RUN OLD TESTS---------------------------------------------------------------------------------------------------
	//Find a server and open a command channel to it
	Picto::ServerDiscoverer serverDiscoverer;
	serverDiscoverer.discover();
	if(!serverDiscoverer.waitForDiscovered(10000))
	{
		qFatal("Server Discoverer failed to discover server.");
		return 1;
	}	
	// Create unit test objects
	TestBasicProtocolCommands basicProtocolCommandTestObject(serverDiscoverer.getAddress(),serverDiscoverer.getPort());
	TestCommandChannel commandChannelTestObject(serverDiscoverer.getAddress(),serverDiscoverer.getPort());
	// Run unit tests
	result +=  QTest::qExec(&basicProtocolCommandTestObject, testOptionList);
	result += QTest::qExec(&commandChannelTestObject, testOptionList);

	// RUN NEW TESTS that use PictoTestBench -------------------------------------------------------------------------
	// Create unit test object
	PictoServerTest serverTest;
	// Run unit test
	result += QTest::qExec(&serverTest, testOptionList);

	return result;
}


// The Original Main method for these tests appears below.  It has been replaced with the main method above.
//int main(int argc, char *argv[])
//{
//    QCoreApplication app(argc, argv);
//
//	QLocale systemLocale = QLocale();
//	QString localeLanguageCode = systemLocale.name().left(2);
//
//	Picto::InitializeLib(&app,localeLanguageCode);
//
//	QHostAddress serverAddress;
//
//	if(argc>1)
//	{
//		QString runTestsTest(argv[1]);
//
//		if(runTestsTest == "RUNTESTS")
//		{
//			if(argc>2)
//			{
//				serverAddress.setAddress(argv[2]);
//			}
//			else
//			{
//				serverAddress.setAddress("192.168.3.100");
//			}
//
//			//TestBasicProtocolCommands basicProtocolCommandTestObject(serverAddress);
//			TestCommandChannel commandChannelTestObject(serverAddress);
//
//			QStringList testOptionList;
//
//			//We first need to add the executable image name so that it can properly be added
//			//as the first argument.  Otherwise, our other arguments will be ignored.
//			testOptionList += argv[0];
//			//testOptionList += "-xml";
//
//			int result=0;
//			//result +=  QTest::qExec(&basicProtocolCommandTestObject, testOptionList);
//			result += QTest::qExec(&commandChannelTestObject, testOptionList);
//			return result;
//		}
//	}
//
//	//I'm not exactly sure what's going on, but somehow the following code
//	//results in a nasty loop spawning processes ad infinitum.  This sucks.
//	//So, rather than risk executing the code, I've simply commented it out.
//
//	/*QString program(argv[0]);
//	QStringList arguments;
//
//	arguments += argv[0];
//	arguments += "RUNTESTS";
//	
//	if(argc>1)
//	{
//		arguments += argv[1];
//	}
//
//	QProcess testSubProcess;
//
//	testSubProcess.start(program,arguments);*/
//	/*! \todo this should actually enter an event loop, and provide feedback while running the tests */
//	/*testSubProcess.waitForFinished(-1);
//	QByteArray results = testSubProcess.readAllStandardOutput();
//
//	//Process results here
//	QTextStream outputStream(stdout);
//	outputStream << results;*/
//
//	return 0;
//}
