/*	As the codebase has evolved, this code has become uncompilable.
	If you want to run this test, it will take some effort to debug...
 */

/*#include <QApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>

#include "EngineTest.h"
#include "../common/compositor/RenderingTarget.h"
#include "compositor/D3DVisualTarget.h"
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
#include "../common/controlelements/ChoiceController.h"

#include "../common/stimuli/VisualElementFactory.h"
#include "../common/stimuli/BoxGraphic.h"
#include "../common/stimuli/EllipseGraphic.h"
#include "../common/stimuli/CircleGraphic.h"

#include "../common/storage/DataStore.h"

#include "../common/engine/MouseSignalChannel.h"
#include "engine/PictoBoxXPAnalogInputSignalChannel.h"

#include "../common/iodevices/RewardController.h"
#include "iodevices/PictoBoxXPRewardController.h"
#include "../common/iodevices/NullRewardController.h"
#include "../common/iodevices/EventCodeGenerator.h"
#include "iodevices/PictoBoxXPEventCodeGenerator.h"
#include "../common/iodevices/NullEventCodeGenerator.h"

#include "../common/parameter/ParameterFactory.h"
#include "../common/parameter/NumericParameter.h"
#include "../common/parameter/BooleanParameter.h"

#include "../common/network/ServerDiscoverer.h"
#include "../common/memleakdetect.h"

//Define this if we're actually running on PictoBox
//#define USING_PICTOBOX

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
	QSharedPointer<Picto::MouseSignalChannel> mouseChannel(new Picto::MouseSignalChannel(10,d3dVisualTarget.data()));
	engine_.addSignalChannel("Position",mouseChannel);

	//Create a rewardController
#ifdef USING_PICTOBOX
	QSharedPointer<Picto::RewardController> rewardController(new Picto::PictoBoxXPRewardController(1));
	QSharedPointer<Picto::EventCodeGenerator> eventGenerator(new Picto::PictoBoxXPEventCodeGenerator());
#else
	QSharedPointer<Picto::RewardController> rewardController(new Picto::NullRewardController());
	QSharedPointer<Picto::EventCodeGenerator> eventGenerator(new Picto::NullEventCodeGenerator());
#endif
	rewardController->setRewardResetTimeMs(1,500);
	engine_.setRewardController(rewardController);

	engine_.setEventCodeGenerator(eventGenerator);

	//Set up all of the factories
	//Set up the VisualElementFactory
	Picto::VisualElementFactory visualElementFactory;
	visualElementFactory.addVisualElementType(Picto::BoxGraphic::type, &Picto::BoxGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::EllipseGraphic::type, &Picto::EllipseGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::CircleGraphic::type, &Picto::CircleGraphic::NewVisualElement);


	//Set up the ControlElementFactory
	Picto::ControlElementFactory controlElementFactory;
	controlElementFactory.addControlElementType(Picto::TestController::ControllerType(), &Picto::TestController::NewTestController);
	controlElementFactory.addControlElementType(Picto::StopwatchController::ControllerType(), &Picto::StopwatchController::NewStopwatchController);
	controlElementFactory.addControlElementType(Picto::TargetController::ControllerType(), &Picto::TargetController::NewTargetController);
	controlElementFactory.addControlElementType(Picto::ChoiceController::ControllerType(), &Picto::ChoiceController::NewChoiceController);

	Picto::ParameterFactory parameterFactory;
	parameterFactory.addParameterType("Numeric",&Picto::NumericParameter::NewParameter);
	parameterFactory.addParameterType("Boolean",&Picto::BooleanParameter::NewParameter);


}


void EngineTest::exec()
{
	Picto::ServerDiscoverer serverDiscoverer;
	QSharedPointer<Picto::CommandChannel> serverChannel(createUuid(),new Picto::CommandChannel);
	serverChannel->pollingMode(true);

	//connect(&serverDiscoverer,SIGNAL(foundServer(QHostAddress, quint16)),
		//serverChannel.data(), SLOT(connectToServer(QHostAddress, quint16)));

	serverDiscoverer.discover();

	//Note that waitForDiscovered runs an event loop for us, so the signals & slots will work temporarily
	if(!serverDiscoverer.waitForDiscovered())
	{
		QMessageBox notFound;
		notFound.setText("No PictoServer found on the network.  PictoDirector will terminate.");
		notFound.setIcon(QMessageBox::Critical);
		notFound.exec();

		return;
	}
	QHostAddress serverAddr = serverDiscoverer.getAddress();
	quint16 port = serverDiscoverer.getPort();

	serverChannel->connectToServer(serverAddr, port);

	Q_ASSERT(serverChannel->getChannelStatus() == Picto::CommandChannel::connected);

	engine_.setCommandChannel(serverChannel);




	//TestLayerRendering();
	TestStateMachine();
}

//! \brief Tests the rendering to screen performed by the Layer object
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
	//QFile stateMachineXML("C:\\Projects\\PictoSVN\\Picto\\trunk\\source\\director\\StateMachineForFrameRateTesting.xml");
	//QFile stateMachineXML("StateMachineForFrameRateTesting.xml");
	//QFile stateMachineXML("C:\\Projects\\PictoSVN\\Picto\\trunk\\source\\director\\StateMachineTest1.xml");
	QFile stateMachineXML("StateMachineTest1.xml");
	Q_ASSERT(stateMachineXML.open(QIODevice::ReadOnly));
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(&stateMachineXML));

	while(xmlStreamReader->name() != "StateMachine")
		xmlStreamReader->readNext();
	Picto::StateMachine stateMachine;
	if(!stateMachine.fromXml(xmlStreamReader))
	{
		QMessageBox error;
		//The extra spaces are to resize the MessageBox
		error.setText("Error reading StateMachine XML                                                         ");
		error.setIcon(QMessageBox::Critical);
		error.setDetailedText(Picto::DataStore::getErrors());
		error.resize(400,100);
		error.exec();
		return;
	}
	if(!stateMachine.validateStateMachine())
	{
		QMessageBox error;
		error.setText("StateMachine XML describes an invalid StateMachine");
		error.setIcon(QMessageBox::Critical);
		error.exec();
		return;
	}


	stateMachine.run();


}*/