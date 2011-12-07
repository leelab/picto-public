/*! \file blankscreen/main.cpp
 *
 *	\brief A really simple embedded apllication that displays a blank screen
 *
 *	This is a simple test application designed to be run on PictoBox.  It was originally
 *	designed so that we could test the Windows CE Platform that we had built, but it is
 *	generally usable as a way to confirm that any device is capable of running Qt.
 */

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDesktopWidget>
#include <QPalette>
#include <QSharedPointer>
#include <QFile>
#include <QHBoxLayout>
//#include <QtVariantPropertyManager>

#ifdef WINCE
#include <winbase.h>
#endif
#include "../../common/common.h"
#include "../../common/globals.h"
#include "../../common/namedefs.h"
#include "../../common/archives/archives.h"
#include "../../common/random/mtrand.h"
#include "../../common/Storage/Asset.h"
#include "../../common/Experiment/Experiment.h"
#include "../../common/parameter/OperatorClickParameter.h"
#include "../../common/storage/propertyfactory.h"
#include "../../common/property/PropertyContainer.h"
#include "../../common/controlelements/stopwatchController.h"
#include "../../common/statemachine/State.h"
#include "../../common/statemachine/StateMachine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/MouseSignalChannel.h"
#include "../../common/mainmemleakdetect.h"

using namespace Picto;

class BlankWindow : public QWidget
{
	QSharedPointer<Asset> test;
public:
    BlankWindow(QWidget *parent = 0) : QWidget(parent)
    {
        QWidget::setWindowFlags(Qt::Window);
        
        QRect screenRect = QApplication::desktop()->screenGeometry(-1);
        resize(screenRect.width(),screenRect.height());
        move(0,0);
        
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::black);
        setPalette(pal);
        
        setFocusPolicy(Qt::ClickFocus);

		//QSharedPointer<Asset> test(StopwatchController::Create());

		QFile file("TestXml.xml");
		file.open(QIODevice::ReadOnly);
		QString fileText = file.readAll();
		
		test = Experiment::Create();
		test->fromXml(fileText);















	//set up the engine
	QSharedPointer<Engine::PictoEngine> engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(true);

	//Set up the rendering target
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	QSharedPointer<Picto::RenderingTarget> renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	Picto::VisualTargetHost* visualTargetHost_ = new Picto::VisualTargetHost();
	visualTargetHost_->setVisualTarget(pixmapVisualTarget_);
	QHBoxLayout *operationLayout = new QHBoxLayout;
	operationLayout->addWidget(visualTargetHost_);
	setLayout(operationLayout);

	//set up mouse signal channel
	QSharedPointer<Picto::MouseSignalChannel> mouseChannel(new Picto::MouseSignalChannel(10,visualTargetHost_));
	engine_->addSignalChannel("PositionChannel",mouseChannel);

	//Set up event code generator
	QSharedPointer<Picto::EventCodeGenerator> nullGenerator;
	nullGenerator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	engine_->setEventCodeGenerator(nullGenerator);

	//set up reward controller
	QSharedPointer<Picto::RewardController> rewardController;
	rewardController = QSharedPointer<Picto::RewardController>(new Picto::AudioRewardController());
	engine_->setRewardController(rewardController);
	test.staticCast<Experiment>()->setEngine(engine_);













		//QSharedPointer<PropertyContainer> propContainer_ = PropertyContainer::create("TestName");
		//QSharedPointer<AssetFactory> propFactory(new PropertyFactory(
		//												0,
		//												10,
		//												propContainer_,
		//												QVariant::Int,
		//												"TestTagName",
		//												0)
		//											);
		//propFactory->startSourcing();
		//QString type = "";
		//QString error = "";
		//QSharedPointer<Asset> newChild = propFactory->getAsset(error,type);
		//newChild = propFactory->getAsset(error,type);
		//newChild = propFactory->getAsset(error,type);
		//newChild = propFactory->getAsset(error,type);

		//propContainer_->addProperty(QVariant::Color,"TestPropertyName",0,false );
		//propContainer_->addProperty(QVariant::Int,"TestPropertyName1",0,false );
		//propContainer_->addProperty(QVariant::Bool,"TestPropertyName2",0,false );
		//propContainer_->addProperty(QVariant::Rect,"TestPropertyName3",0,false );
		//propContainer_->addProperty(QVariant::String,"TestPropertyName4",0,false );
		//propContainer_->addProperty(QVariant::Double,"TestPropertyName5",0,false );
		//QSharedPointer<Property> prop = propContainer_->addProperty(QtVariantPropertyManager::enumTypeId(),"TestPropertyName6",0,false );
		//prop->setAttribute("0",0);
		//prop->setAttribute("1",1);
		//prop->setAttribute("2",2);



		int i=0;
		i++;
    }

	void play()
	{
		test.staticCast<Experiment>()->runTask("ProbabilityTracking");
	}
    void focusInEvent(QFocusEvent *)
    {
        lower();
    }
};

int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	QApplication::setDesktopSettingsAware(false);
	QApplication a(argc, argv);

	//QString* memLeakStr = new QString();
  BlankWindow blankWindow;
  blankWindow.show();
  blankWindow.play();

  if(argc>1)
  {
#ifdef WINCE
     SignalStarted(atol(argv[1]));
#endif
  }

	return a.exec();
}
