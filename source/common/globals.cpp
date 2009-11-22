#include "common.h"
#include "globals.h"
#include "namedefs.h"

#include <QtCore/QTranslator>

#include <QMetaType>
#include <QHostAddress>
#include <QMap>
#include <QString>
#include <QVariant>

Q_DECLARE_METATYPE(QHostAddress);

namespace Picto {

const TranslatedNames * Names = NULL;
QTranslator * qtTranslator = NULL;
QTranslator * commonTranslator = NULL;

void InitializeMetaTypes()
{
	qRegisterMetaType<QHostAddress>();
	qRegisterMetaType<QMap<QString, QVariant> >();
}

void InitializeLib(QCoreApplication * coreApp, QString localeLanguageCode)
{
	if(coreApp)
	{
		qtTranslator = new QTranslator();
		qtTranslator->load(":/common/translations/qt_" + localeLanguageCode + ".qm" );
		coreApp->installTranslator(qtTranslator);

		commonTranslator = new QTranslator();
		commonTranslator->load(":/common/translations/common_" + localeLanguageCode + ".qm" );
		coreApp->installTranslator(commonTranslator);
	}

	Names = new TranslatedNames();

	InitializeMetaTypes();
}

void CloseLib()
{
	delete Names;
	delete commonTranslator;
	delete qtTranslator;
	Names = NULL;
	commonTranslator = NULL;
	qtTranslator = NULL;
}

}; //namespace Picto
