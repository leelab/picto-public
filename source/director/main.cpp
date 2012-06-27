/*!	\file director/main.cpp
 *	\brief PictoDirector is the software that presents the stimuli and collects the behavioral data.
 *
 *	Picto Director is one of the major components of the Picto system.  This software
 *	is responsible for presenting the stimuli, collecting the behavioral data, and 
 *	controlling the flow of the experiment by running the state machines.  Director
 *	is also responsible for all communications with PictoServer.
 *	
 *	Despite the long list of responsibilities, PictoDirector is actually a really
 *	simple program.  Upon booting it first sets up hardware to match the system on 
 *	which it is running (actually, at the moment it sets up the hardware for PictoBoxXp).
 *	Then, it attempts to connect to a PictoServer instance. Once connected, it sits in a 
 *	loop waiting sending COMPONENTUPDATE commands.  The responses to these commands may 
 *	include "directives" which tell Director to do something (start an experiment, deliver
 *	a reward, etc).
 *
 *	In its current state, PictoDirector is really only capable of running on PictoBox
 *	with WinXP.  However, this was not how the application was intended to work.  In
 *	theory, PictoDirector should be able to run on PictoBox with WinCE, or on any machine.
 *	To get this working, you'll need to create some sort of PictoDirector configuration 
 *	scheme.  A basic one of these has already been developed using command line arguments.
 *
 *	Command line arguments:
 *	- -name: Set the name of this Director instance to the string argument immediately following "-name"
 *	- -pixmap: Run Director using Pixmap rendering.  This is really slow, but it lets you run
 *	Director in places where DirectX is not available (e.g. when running Director on a machine that
 *	is accessed by remote desktop).
 */

#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSharedPointer>

#include "../common/globals.h"

#include "HardwareSetup.h"

#include "Director.h"
#include "../common/mainmemleakdetect.h"

QSharedPointer<Picto::CommandChannel> connectToServer(QUuid directorID);
void setUserInfo(QSharedPointer<Picto::Engine::PictoEngine> engine, QString status);


int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
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

	Picto::InitializeLib(&app,localeLanguageCode);



//possibly reset our name...
	QStringList args = app.arguments();
	int nameArgIdx = args.indexOf("-name");
	QString newName = "";
	if(nameArgIdx > 0)
		newName = args[nameArgIdx+1];

	
	//If there is a command argument of "-pixmap", we should use a pixmpa rendering
	//otherwise use d3d.
	HardwareSetup::VisualTargetType visTarget = HardwareSetup::D3D;
	if(app.arguments().contains("-pixmap"))
		visTarget = HardwareSetup::Pixmap;

	//If there is a command of "-xChan", "-yChan" we should use an eye tracker.
	//and set the appropriate channels.  Otherwise, just use the mouse.
	HardwareSetup::SignalChannelType sigChan = HardwareSetup::Mouse;
	int chanArgIdx = args.indexOf("-xChan");
	int xChan = 0;
	if(chanArgIdx > 0)
	{
		xChan = args[chanArgIdx+1].toInt();
		sigChan = HardwareSetup::EyetrackerPictoBoxXp;
	}
	chanArgIdx = args.indexOf("-yChan");
	int yChan = 1;
	if(chanArgIdx > 0)
	{
		yChan = args[chanArgIdx+1].toInt();
		sigChan = HardwareSetup::EyetrackerPictoBoxXp;
	}

	//Check if pupilDiameter should be recorded and on which channels.
	int diamChanArgIdx = args.indexOf("-xDiamChan");
	int xDiamChan = -1;
	if(diamChanArgIdx > 0)
	{
		xDiamChan = args[diamChanArgIdx+1].toInt();
#ifndef DEVELOPMENTBUILD
		if(sigChan == HardwareSetup::Mouse)
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
	if(diamChanArgIdx > 0)
	{
		yDiamChan = args[diamChanArgIdx+1].toInt();
#ifndef DEVELOPMENTBUILD
		if(sigChan == HardwareSetup::Mouse)
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
	if(posPerArgIdx > 0)
	{
		posPer = args[posPerArgIdx+1].toInt();
	}

	//If there is a command of -diamPeriod use it to configure the number of ms per 
	//eye diameter reading
	int diamPerArgIdx = args.indexOf("-diamPeriod");
	int diamPer = 4;
	if(diamPerArgIdx > 0)
	{
		diamPer = args[diamPerArgIdx+1].toInt();
	}

	//If there is a command argument of "-legacy", we should use 
	//LegacySystemXPRewardController and LegacySystenXPEventCodeGenerator
	//otherwise use NullReward.
	//We should also switch an eye tracking signal channel to the 
	//legacy system's eye tracker.
	HardwareSetup::RewardControllerType rewCont = HardwareSetup::NullReward;
	HardwareSetup::EventCodeGeneratorType eventGen = HardwareSetup::NullGen;
	if(app.arguments().contains("-legacy"))
	{
		rewCont = HardwareSetup::LegacySystemXpReward;
		eventGen = HardwareSetup::LegacyGen;
		if(sigChan != HardwareSetup::Mouse)
			sigChan = HardwareSetup::EyetrackerLegacySystemXp;
	}
	else if(app.arguments().contains("-pictobox"))
	{
		rewCont = HardwareSetup::PictoBoxXpReward;
	}
	else if(app.arguments().contains("-test"))
	{
		rewCont = HardwareSetup::AudioReward;
	}


	//For now, just use pixmap ------- Remove this/////////////////////
	//visTarget = HardwareSetup::Pixmap;
	///////////////////////////////////////////////////////////////////	

	QSharedPointer<Director> director(new Director(newName,sigChan,visTarget,rewCont,eventGen,xChan,yChan,xDiamChan,yDiamChan,posPer,diamPer));
	director->activate();
	Picto::CloseLib();
	return EXIT_SUCCESS;
}
