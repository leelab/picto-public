/*!	\file director/main.cpp
*	\brief PictoDirector is the application that presents stimuli and collects the behavioral data.  It
*	runs the Picto "game."
*
*	Picto Director is one of the major components of the Picto system.  This software
*	is responsible for presenting Experimental stimuli, collecting behavioral data from the test
*	subject, and controlling the flow of the experiment by running the StateMachine.  The Director
*	communicates with PictoServer during the experiment, receives commands and sends over all Experimental
*	data in real time.
*
*	Upon booting, the Director first sets up the hardware interface to match the system on
*	which it is running (currently this is limited to a PictoBox or a legacy Orion computer).
*	Next, it attempts to connect to a PictoServer instance. Once connected, it sits in a
*	loop sending COMPONENTUPDATE "keep alive" commands periodically.  The responses to these commands may
*	include "directives" which tell the Director to do something (start an experiment, deliver
*	a reward, etc).
*
*	In its current state, PictoDirector is really only capable of running on Windows.  However, this was
*	not how the application was intended to work.  In theory, PictoDirector should be able to run on
*	other Operating systems as well.  If QT supported everything that we wanted to do, this would be
*	trivial.  In practice though, we need to communicate with the NiDaq card, the Phidgets devices,
*	and render at high speed with precise timing to the display.  None of these operations are supporte
*	in Qt, so supporting other OSes will necessitate writing the classes that handle those operations
*	for the other OS.
*
*	Command line arguments:
*	- -name: Set the name of this Director instance to the string argument immediately following "-name "
*  - -systemNumber: Sets the system number to the number immediately following "-systemNumber ".  A
*		Director can communicate only with a PictoServer running with the same system number
*  - -pixmap: If this is included, Pixmap rendering will be used instead of DirectX rendering.  This makes
*		timing less precise, but is good for testing since it is a little kinder about things like taking over
*		as the main window and generally allowing for other things to happen on the Director computer.  It
*		is also usable over remote desktop.
*  - -xChan: The number argument following this one defines the analog input channel on which the x coordinate of eye position data will come in.
*		If xChan and yChan are not used, the mouse will be used for position input.
*  - -yChan: The number argument following this one defines the analog input channel on which the y coordinate of eye position data will come in.
*		If xChan and yChan are not used, the mouse will be used for position input.
*  - -xDiamChan: The number argument following this one defines the analog input channel on which the x coordinate of eye diameter data will come in.
*		Don't use this if eye diameter is not tracked.
*  - -yDiamChan: The number argument following this one defines the analog input channel on which the y coordinate of eye diameter data will come in.
*		Don't use this if eye diameter is not tracked.
*  - -positionPeriod: The number argument following this one defines the sample period per eye position sample.  For example,
*		"-positionPeriod 2" sets up the Director to sample eye position data once every 2 ms.
*  - -diamPeriod: The number argument following this one defines the sample period per eye diameter sample.  For example,
*		"-diamPeriod 4" sets up the Director to sample eye diameter data once every 4 ms.
*  - -legacy: Sets the Director to interface with Orion computer hardware for its RewardController, EventCodeGenerator and
*		AnalogInputPort (no OutputSignalController is supported).  Note that the hardware line that controlles the reward box still needs to be moved to a different
*		pin that was used in Orion.  See LegacySystemXPRewardController for more details.  Use only one of
*		legacy, pictobox, pictocomp, or test.
*  - -pictobox: Sets the Director to interface with Pictobox computer hardware for its RewardController, EventCodeGenerator,
*		AnalogInputPort, and OutputSignalController. Use only one of legacy, pictobox, pictocomp, or test.
*  - -pictocomp: Sets the Director to connect to an Arduino for its RewardController, and to use its hardware for its EventCodeGenerator, AnalogInputPort, 
*		and OutputSignalController. Use only one of legacy, pictobox, pictocomp, or test.
*  - -test: Sets the Director to use an AudioRewardController, MouseSignalChannel and no event or output controllers. Use only one of
*		legacy, pictobox, pictocomp, or test.
*  - -onesided: When this is used there will be no alignment or gathering of neural data.  This means that the parallel port usually used to
*		send alignment events to the neural system will be available as a secondary OutputSignal port to the experiment developer
*		referred to as PAR0.
*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSharedPointer>

#include "../common/globals.h"
#include "../common/update/updatedownloader.h"
#include "HardwareSetup.h"

#include "Director.h"
#include "../common/mainmemleakdetect.h"

QSharedPointer<Picto::CommandChannel> connectToServer(QUuid directorID);
void setUserInfo(QSharedPointer<Picto::Engine::PictoEngine> engine, QString status);

/*! \brief The main method of the Picto Director application.
*	\details Goes through standard Picto application setup procedure, setting up the QApplication,
*	the QTranslator, and calling Picto::InitializeLib() and Picto::IniitializePorts(), then parses
*	the command line for Hardware setup data which will be used in the construction of the
*	Director object.  After parsing command line options, the AutoUpdater system is setup with the
*	commands that will be needed to restart this application, then the Director object is created
*	and its Director::activate() function is called.
*
*	Director::activate() doesn't return until the application closes.
*/
int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	///////////////////////////////////////
	// Setup app
	///////////////////////////////////////
	QApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/director_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app, localeLanguageCode);
	Picto::InitializePorts(Picto::Names->directorAppName);


	//possibly reset our name...
	QStringList args = app.arguments();
	int nameArgIdx = args.indexOf("-name");
	QString newName = "";
	if (nameArgIdx > 0)
		newName = args[nameArgIdx + 1];

	//possibly reset our system number...
	int sysNumArgIdx = args.indexOf("-systemNumber");
	if (sysNumArgIdx > 0)
	{
		QString systemNumber = args[sysNumArgIdx + 1];
		//Change System number if necessary
		Picto::portNums->setSystemNumber(app.applicationFilePath(), app.arguments(), systemNumber.toInt(), false);
	}

	//legacyPhidgets (version 21, picto boxes built before 2018) vs new phidgets (version 22, 2018 and later)
	int legPhidgets = 1;
	int legArgIdx = app.arguments().indexOf("-legacyPhidgets");
	if (legArgIdx > 0)
	{
		legPhidgets = (app.arguments()[legArgIdx + 1]).toInt();
	}

	//If there is a command argument of "-pixmap", we should use a pixmpa rendering
	//otherwise use d3d.
	HardwareSetup::VisualTargetType visTarget = HardwareSetup::D3D;
	if (app.arguments().contains("-pixmap"))
		visTarget = HardwareSetup::Pixmap;

	//If there is a command of "-xChan", "-yChan" we should use an eye tracker.
	//and set the appropriate channels.  Otherwise, just use the mouse.
	HardwareSetup::SignalChannelType sigChan = HardwareSetup::Mouse;
	int chanArgIdx = args.indexOf("-xChan");
	int xChan = 0;
	if (chanArgIdx > 0)
	{
		xChan = args[chanArgIdx + 1].toInt();
		sigChan = HardwareSetup::EyetrackerPictoBoxXp;
	}
	chanArgIdx = args.indexOf("-yChan");
	int yChan = 1;
	if (chanArgIdx > 0)
	{
		yChan = args[chanArgIdx + 1].toInt();
		sigChan = HardwareSetup::EyetrackerPictoBoxXp;
	}

	//Check if pupilDiameter should be recorded and on which channels.
	int diamChanArgIdx = args.indexOf("-xDiamChan");
	int xDiamChan = -1;
	if (diamChanArgIdx > 0)
	{
		xDiamChan = args[diamChanArgIdx + 1].toInt();
#ifndef DEVELOPMENTBUILD
		if (sigChan == HardwareSetup::Mouse)
		{
			QMessageBox error;
			//The extra spaces are to resize the MessageBox
			error.setText("Error setting up Picto Director");
			error.setIcon(QMessageBox::Critical);
			error.setDetailedText("Pupil Diameter is not available when the mouse is used for the position channel");
			error.exec();
			return EXIT_FAILURE;
		}
#endif
	}
	diamChanArgIdx = args.indexOf("-yDiamChan");
	int yDiamChan = -1;
	if (diamChanArgIdx > 0)
	{
		yDiamChan = args[diamChanArgIdx + 1].toInt();
#ifndef DEVELOPMENTBUILD
		if (sigChan == HardwareSetup::Mouse)
		{
			QMessageBox error;
			//The extra spaces are to resize the MessageBox
			error.setText("Error setting up Picto Director");
			error.setIcon(QMessageBox::Critical);
			error.setDetailedText("Pupil Diameter is not available when the mouse is used for the position channel");
			error.exec();
			return EXIT_FAILURE;
		}
#endif
	}

	//If there is a command of -positionPeriod use it to configure the number of ms per 
	//eye position reading
	int posPerArgIdx = args.indexOf("-positionPeriod");
	int posPer = 2;
	if (posPerArgIdx > 0)
	{
		posPer = args[posPerArgIdx + 1].toInt();
	}

	//If there is a command of -diamPeriod use it to configure the number of ms per 
	//eye diameter reading
	int diamPerArgIdx = args.indexOf("-diamPeriod");
	int diamPer = 4;
	if (diamPerArgIdx > 0)
	{
		diamPer = args[diamPerArgIdx + 1].toInt();
	}

	//If there is a command of -bgColor use it to figure out the version of phidgets api to call
	int bgColRArgIdx = app.arguments().indexOf("-R");
	int bgColR = 0;
	if (bgColRArgIdx > 0)
	{
		bgColR = app.arguments()[bgColRArgIdx + 1].toInt();
	}

	//If there is a command of -bgColor use it to figure out the version of phidgets api to call
	int bgColGArgIdx = app.arguments().indexOf("-G");
	int bgColG = 0;
	if (bgColGArgIdx > 0)
	{
		bgColG = app.arguments()[bgColGArgIdx + 1].toInt();
	}

	//If there is a command of -bgColor use it to figure out the version of phidgets api to call
	int bgColBArgIdx = app.arguments().indexOf("-B");
	int bgColB = 0;
	if (bgColBArgIdx > 0)
	{
		bgColB = app.arguments()[bgColBArgIdx + 1].toInt();
	}
	QColor bgColor(bgColR, bgColG, bgColB, 255);

	//If there is a command argument of "-legacy", we should use 
	//LegacySystemXPRewardController and LegacySystenXPEventCodeGenerator
	//otherwise use NullReward.
	//We should also switch an eye tracking signal channel to the 
	//legacy system's eye tracker.
	HardwareSetup::RewardControllerType rewCont = HardwareSetup::NullReward;
	HardwareSetup::EventCodeGeneratorType eventGen = HardwareSetup::NullGen;
	HardwareSetup::OutputSignalControllerType outSigCont = HardwareSetup::NullOutSig;
	bool frontPanel = false;
	if (app.arguments().contains("-legacy"))
	{
		rewCont = HardwareSetup::LegacySystemXpReward;
		eventGen = HardwareSetup::LegacyGen;
		if (sigChan != HardwareSetup::Mouse)
			sigChan = HardwareSetup::EyetrackerLegacySystemXp;
	}
	else if (app.arguments().contains("-pictobox"))
	{
		rewCont = HardwareSetup::PictoBoxXpReward;
		eventGen = HardwareSetup::PictoBoxXpGen;
		outSigCont = HardwareSetup::PictoBoxXpOutSig;
		frontPanel = true;
	}
	else if (app.arguments().contains("-pictocomp"))
	{
		rewCont = HardwareSetup::PictoCompReward;
		eventGen = HardwareSetup::PictoBoxXpGen;
		outSigCont = HardwareSetup::PictoBoxXpOutSig;
	}
	else if (app.arguments().contains("-test"))
	{
		rewCont = HardwareSetup::AudioReward;
	}

	if (app.arguments().contains("-onesided"))
	{	//One Sided means that there will be no alignment or gathering of neural data.
		//The port usually used to send alignment events to the neural system will be
		//available as a secondary port to the experiment developer.
		eventGen = HardwareSetup::NullGen;
	}

	//Setup autoupdate system
	UpdateDownloader::getInstance()->setRestartCommands(app.applicationFilePath(), app.arguments());

	QSharedPointer<Director> director(new Director(newName, sigChan, visTarget, rewCont, outSigCont, eventGen, xChan, yChan, xDiamChan, yDiamChan, posPer, diamPer, bgColor, legPhidgets, frontPanel));
	director->activate();
	Picto::CloseLib();
	return EXIT_SUCCESS;
}
