/*!	\file common/unittests/main.cpp
 *	\brief These unit tests are used for testing libPicto.
 *
 *	Ummmmm, yeah, unit testing....
 *
 *	So, some of these unit tests worked at one time.  However, I wasn't
 *	religious about testing, and it's possible that code that previously
 *	passed will now fail.  I also made some changes that were dramatic enough
 *	to prevent the tests from compiling.  Those tests got commented out.
 */

#include <QLocale>
#include <QApplication>
#include <QCoreApplication>
#include <QtTest>

#include "../../common/common.h"
#include "../../common/globals.h"
//#include "TestPredicates.h"
#include "TestDataStore.h"
#include "TestParameters.h"
#include "TestStateMachine.h"
#include "TestScripts.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	Picto::InitializeLib(&app,localeLanguageCode);
	
	//TestPredicates testPredicatesObject;
	TestDataStore testDataStoreObject;
	TestParameters testParametersObject;
	TestStateMachine testStateMachineObject;
	TestScripts testScriptsObject;

	int result = 0;
	//result += QTest::qExec((QObject*)&testPredicatesObject);
	result += QTest::qExec((QObject*)&testDataStoreObject);
	result += QTest::qExec((QObject*)&testStateMachineObject);
	result += QTest::qExec((QObject*)&testParametersObject);
	result += QTest::qExec((QObject*)&testScriptsObject);

	return result;
}

