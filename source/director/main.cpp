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
#include <QSharedPointer>

#include "../common/globals.h"

#include "HardwareSetup.h"

#include "Director.h"

QSharedPointer<Picto::CommandChannel> connectToServer(QUuid directorID);
void updateSplashStatus(QSharedPointer<Picto::Engine::PictoEngine> engine, QString status);


int main(int argc, char *argv[])
{

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

	//For now, just use pixmap ------- Remove this/////////////////////
	//visTarget = HardwareSetup::Pixmap;
	///////////////////////////////////////////////////////////////////	

	QSharedPointer<Director> director(new Director(newName,HardwareSetup::Mouse,visTarget,HardwareSetup::NullReward,HardwareSetup::NullGen));
	director->activate();
	return 0;
}
