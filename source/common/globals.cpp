#include "common.h"
#include "globals.h"
#include "namedefs.h"

#include <QtCore/QTranslator>

#include <QMetaType>
#include <QHostAddress>
#include <QMap>
#include <QString>
#include <QVariant>

#include "../common/stimuli/ArrowGraphic.h"
#include "../common/stimuli/BoxGraphic.h"
#include "../common/stimuli/CircleGraphic.h"
#include "../common/stimuli/EllipseGraphic.h"
#include "../common/stimuli/LineGraphic.h"
#include "../common/stimuli/PictureGraphic.h"
#include "../common/stimuli/RandomlyFilledGridGraphic.h"
#include "../common/stimuli/TextGraphic.h"

#include "../common/controlelements/TestController.h"
#include "../common/controlelements/StopwatchController.h"
#include "../common/controlelements/TargetController.h"
#include "../common/controlelements/ChoiceController.h"
#include "../common/controlelements/FixationController.h"

#include "../common/parameter/BooleanParameter.h"
#include "../common/parameter/NumericParameter.h"
#include "../common/parameter/RangeParameter.h"
#include "memleakdetect.h"


Q_DECLARE_METATYPE(QHostAddress);

namespace Picto {

const TranslatedNames * Names = NULL;
PortNums * portNums = NULL;
QTranslator * qtTranslator = NULL;
QTranslator * commonTranslator = NULL;

/*! \brief Registers application names in the global "Names" variable.*/
void InitializeNames()
{
	if(!Names)
		Names = new TranslatedNames();
}

/*! \brief Registers types that need to be used in signals and slots with the Qt Framework.*/
void InitializeMetaTypes()
{
	qRegisterMetaType<QHostAddress>();
	qRegisterMetaType<QMap<QString, QVariant> >();
	qRegisterMetaType<QSharedPointer<Asset>>("QSharedPointer<Asset>");
}

/*! \brief Initializes the %Picto library
 *
 * InitializeLib must be called by code using the %Picto library prior to usage.  This
 * function sets up the library's state for proper execution.  It takes care of calling 
 * all the "initialize" functions above.
 */
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
	InitializeNames();
	InitializeMetaTypes();
}

/*! \brief Initializes the ports used by this %Picto application according to the system number
 *
 *	InitializePorts() must be called by code using the %Picto library prior to usage and after calling
 *	InitializeLib().  This function sets up the port numbers used by Picto to communicate with other
 *	%picto applications.  The input name should come from the Names object that is set up in
 *	InitializeLib().
 *	\note When the Picto Server runs as a service, it does not have access to the regular Picto settings
 *	so they need to be saved in the System Scope.  This is the purpose of the usedInSystemServices parameter.
 *	All other Picto applications should save their values in the user scope (because setting system settings requires administrator previleges).
 */
void InitializePorts(QString appName,bool usedInSystemService)
{
	if(!portNums)
		portNums = new PortNums(appName,usedInSystemService);
}

/*! \brief Shuts down the %Picto library
 *
 * \details CloseLib must be called by code using the %Picto library prior to shutdown.  This
 * function performs any needed house-keeping prior to closure.
 */
void CloseLib()
{
	delete Names;
	delete portNums;
	delete commonTranslator;
	delete qtTranslator;
	Names = NULL;
	portNums = NULL;
	commonTranslator = NULL;
	qtTranslator = NULL;
}

}; //namespace Picto
