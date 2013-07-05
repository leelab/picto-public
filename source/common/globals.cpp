#include "common.h"
#include "globals.h"
#include "namedefs.h"

#include <QtCore/QTranslator>

#include <QMetaType>
#include <QHostAddress>
#include <QMap>
#include <QString>
#include <QVariant>

#include "../common/stimuli/VisualElementFactory.h"
#include "../common/stimuli/ArrowGraphic.h"
#include "../common/stimuli/BoxGraphic.h"
#include "../common/stimuli/CircleGraphic.h"
#include "../common/stimuli/EllipseGraphic.h"
#include "../common/stimuli/LineGraphic.h"
#include "../common/stimuli/PictureGraphic.h"
#include "../common/stimuli/RandomlyFilledGridGraphic.h"
#include "../common/stimuli/TextGraphic.h"

#include "../common/controlelements/ControlElementFactory.h"
#include "../common/controlelements/TestController.h"
#include "../common/controlelements/StopwatchController.h"
#include "../common/controlelements/TargetController.h"
#include "../common/controlelements/ChoiceController.h"

#include "../common/parameter/ParameterFactory.h"
#include "../common/parameter/BooleanParameter.h"
#include "../common/parameter/ChoiceParameter.h"
#include "../common/parameter/NumericParameter.h"
#include "../common/parameter/RangeParameter.h"
#include "memleakdetect.h"


Q_DECLARE_METATYPE(QHostAddress);

namespace Picto {

const TranslatedNames * Names = NULL;
PortNums * portNums = NULL;
QTranslator * qtTranslator = NULL;
QTranslator * commonTranslator = NULL;

void InitializeNames()
{
	if(!Names)
		Names = new TranslatedNames();
}

void InitializeMetaTypes()
{
	qRegisterMetaType<QHostAddress>();
	qRegisterMetaType<QMap<QString, QVariant> >();
	qRegisterMetaType<QSharedPointer<Asset>>("QSharedPointer<Asset>");
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
	InitializeNames();
	InitializeMetaTypes();
	initializeFactories();
}

//When the Picto Server runs as a picto service, it does not have access to the regular Picto settings
//so they need to be saved in the System Scope.  All other Picto applications should save their values
//in the user scope (because setting system settings requires administrator previleges).
void InitializePorts(QString appName,bool usedInSystemService)
{
	if(!portNums)
		portNums = new PortNums(appName,usedInSystemService);
}

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

//New factories and their setup stuff should be included here...
void initializeFactories()
{
	////Set up the VisualElementFactory
	//Picto::VisualElementFactory visualElementFactory;
	//visualElementFactory.addVisualElementType(Picto::ArrowGraphic::type, &Picto::ArrowGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::BoxGraphic::type, &Picto::BoxGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::CircleGraphic::type, &Picto::CircleGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::EllipseGraphic::type, &Picto::EllipseGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::LineGraphic::type, &Picto::LineGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::PictureGraphic::type, &Picto::PictureGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::RandomlyFilledGridGraphic::type, &Picto::RandomlyFilledGridGraphic::NewVisualElement);
	//visualElementFactory.addVisualElementType(Picto::TextGraphic::type, &Picto::TextGraphic::NewVisualElement);

	////Set up the ControlElementFactory
	//Picto::ControlElementFactory controlElementFactory;
	//controlElementFactory.addControlElementType(Picto::TestController::ControllerType(), &Picto::TestController::NewTestController);
	//controlElementFactory.addControlElementType(Picto::StopwatchController::ControllerType(), &Picto::StopwatchController::NewStopwatchController);
	//controlElementFactory.addControlElementType(Picto::TargetController::ControllerType(), &Picto::TargetController::NewTargetController);
	//controlElementFactory.addControlElementType(Picto::ChoiceController::ControllerType(), &Picto::ChoiceController::NewChoiceController);

	////Set up the Parameter Factory
	//Picto::ParameterFactory parameterFactory;
	//parameterFactory.addParameterType("Boolean",&Picto::BooleanParameter::NewParameter);
	//parameterFactory.addParameterType("Choice",&Picto::ChoiceParameter::NewParameter);
	//parameterFactory.addParameterType("Numeric",&Picto::NumericParameter::NewParameter);
	//parameterFactory.addParameterType("Range",&Picto::RangeParameter::NewParameter);

}

}; //namespace Picto
