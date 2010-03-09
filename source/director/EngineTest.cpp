#include <QApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>

#include "EngineTest.h"
#include "../common/compositor/RenderingTarget.h"
#include "../common/compositor/D3DVisualTarget.h"
#include "../common/compositor/PCMAuralTarget.h"

#include "../common/statemachine/Layer.h"
#include "../common/statemachine/Canvas.h"
#include "../common/statemachine/Scene.h"
#include "../common/statemachine/State.h"
#include "../common/statemachine/StateMachine.h"

#include "../common/controlelements/ControlElementFactory.h"
#include "../common/controlelements/TestController.h"
#include "../common/controlelements/StopwatchController.h"
#include "../common/controlelements/TargetController.h"

#include "../common/stimuli/VisualElementFactory.h"
#include "../common/stimuli/BoxGraphic.h"

#include "../common/storage/DataStore.h"

#include "../common/engine/MouseSignalChannel.h"
#include "../common/engine/PictoBoxXPAnalogInputSignalChannel.h"

#include "../common/iodevices/PictoBoxXPRewardController.h"

#include "../common/parameter/ParameterFactory.h"
#include "../common/parameter/NumericParameter.h"

EngineTest::EngineTest()
{
	randGen_.seed();

	//Create a RenderingTarget
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	QSharedPointer<Picto::D3DVisualTarget> d3dVisualTarget(new Picto::D3DVisualTarget());

	QSharedPointer<Picto::RenderingTarget> renderingTarget(new Picto::RenderingTarget(d3dVisualTarget, pcmAuralTarget));

	d3dVisualTarget->show();

	//add the rendering target to the engine
	engine_.addRenderingTarget(renderingTarget);

	//Create a mouse signal channel
	QSharedPointer<Picto::MouseSignalChannel> mouseChannel(new Picto::MouseSignalChannel(10,d3dVisualTarget));
	engine_.addSignalChannel("PositionChannel",mouseChannel);

	//Create a rewardController
	//QSharedPointer<Picto::PictoBoxXPRewardController> rewardController(new Picto::PictoBoxXPRewardController(1));
	//rewardController->setRewardResetTimeMs(1,500);
	//engine_.setRewardController(rewardController);

	//Set up all of the factories
	//Set up the VisualElementFactory
	Picto::VisualElementFactory visualElementFactory;
	visualElementFactory.addVisualElementType(Picto::BoxGraphic::type, &Picto::BoxGraphic::NewVisualElement);


	//Set up the ControlElementFactory
	Picto::ControlElementFactory controlElementFactory;
	controlElementFactory.addControlElementType(Picto::TestController::ControllerType(), &Picto::TestController::NewTestController);
	controlElementFactory.addControlElementType(Picto::StopwatchController::ControllerType(), &Picto::StopwatchController::NewStopwatchController);
	controlElementFactory.addControlElementType(Picto::TargetController::ControllerType(), &Picto::TargetController::NewTargetController);

	Picto::ParameterFactory parameterFactory;
	parameterFactory.addParameterType("Numeric",&Picto::NumericParameter::NewParameter);


}

void EngineTest::exec()
{
	//TestLayerRendering();
	TestStateMachine();
}

/*! \brief Tests the rendering to screen performed by the Layer object */
void EngineTest::TestLayerRendering()
{
	//LAYERS
	QSharedPointer<Picto::Layer> layer1(new Picto::Layer);
	QSharedPointer<Picto::BoxGraphic> boxGraphic1(new Picto::BoxGraphic(QPoint(30,30),QRect(0,0,50,100),QColor(255,255,0,127)));
	layer1->addVisualElement(boxGraphic1);
	layer1->setOrder(1);

	QSharedPointer<Picto::Layer> layer2(new Picto::Layer);
	QSharedPointer<Picto::BoxGraphic> boxGraphic2(new Picto::BoxGraphic(QPoint(70,100),QRect(0,0,50,100),QColor(255,0,255,127)));
	layer2->addVisualElement(boxGraphic2);
	layer2->setOrder(2);

	//CANVAS
	QSharedPointer<Picto::Canvas> canvas1(new Picto::Canvas);
	canvas1->addLayer(layer1);

	//SCENE
	QSharedPointer<Picto::Scene> scene1(new Picto::Scene);
	scene1->setCanvas(canvas1);

	//STATE
	QSharedPointer<Picto::State> state1(new Picto::State);
	QSharedPointer<Picto::TestController> testController(new Picto::TestController);
	state1->addControlElement(testController);
	state1->setScene(scene1);
	state1->run();

	QApplication::exit();
}

void EngineTest::TestStateMachine()
{
	//Read in a state machine from file
	//QFile stateMachineXML("C:\\Projects\\PictoSVN\\Picto\\trunk\\source\\director\\StateMachineTest1.xml");
	QFile stateMachineXML("StateMachineTest1.xml");
	Q_ASSERT(stateMachineXML.open(QIODevice::ReadOnly));
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(&stateMachineXML));

	while(xmlStreamReader->name() != "StateMachine")
		xmlStreamReader->readNext();
	Picto::StateMachine stateMachine;
	if(!stateMachine.deserializeFromXml(xmlStreamReader))
	{
		QMessageBox error;
		//The extra spaces are to resize the MessageBox
		error.setText("Error reading StateMachine XML                                                         ");
		error.setIcon(QMessageBox::Critical);
		error.setDetailedText(Picto::DataStore::getErrors());
		error.resize(400,100);
		error.exec();
		QApplication::exit();
		return;
	}
	if(!stateMachine.validateTransitions())
	{
		QMessageBox error;
		error.setText("StateMachine XML describes an invalid StateMachine");
		error.setIcon(QMessageBox::Critical);
		error.exec();
		QApplication::exit();
		return;
	}


	stateMachine.run();


}