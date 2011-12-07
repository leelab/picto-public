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
	initializeFactories();
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
