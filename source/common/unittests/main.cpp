#include <QLocale>
#include <QApplication>
#include <QCoreApplication>
#include <QtTest>

#include "../../common/common.h"
#include "../../common/globals.h"
#include "TestPredicates.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	Picto::InitializeLib(&app,localeLanguageCode);
	
	TestPredicates testPredicatesObject;
	int result = 0;
	result += QTest::qExec((QObject*)&testPredicatesObject);
	
	return result;
}

