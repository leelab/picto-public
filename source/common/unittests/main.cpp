#include <QLocale>
#include <QApplication>
#include <QCoreApplication>
#include <QtTest>

#include "../../common/common.h"
#include "../../common/globals.h"
#include "TestPredicates.h"
#include "TestDataStore.h"
#include "TestParameters.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	Picto::InitializeLib(&app,localeLanguageCode);
	
	TestPredicates testPredicatesObject;
	TestDataStore testDataStoreObject;
	TestParameters testParametersObject;

	int result = 0;
	//result += QTest::qExec((QObject*)&testPredicatesObject);
	result += QTest::qExec((QObject*)&testDataStoreObject);
	//result += QTest::qExec((QObject*)&testParametersObject);
	
	return result;
}

