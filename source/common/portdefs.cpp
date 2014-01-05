#include <QSettings>
#include <QProcess>
#include "portdefs.h"
#include "memleakdetect.h"

namespace Picto {

/*!	\brief Creates a new PortNums object for the application with the input name.
 *	@param usedInSystemService When the Picto Server runs as a service, it does not have access to the regular Picto settings
 *	so they need to be saved in the System Scope.  All other Picto applications should save their values
 *	in the user scope (because setting system settings requires administrator previleges).
 */
PortNums::PortNums(QString appName,bool usedInSystemService)
:
usedInSystemService_(usedInSystemService)
{
	appName_ = appName;
	systemNumber_ = 1;
	updatePortNumbers();
}

/*! \brief Sets the system number for this system internally and in the QSettings saved configuration.  
 *	\details Picto applications can only communicate with other Picto applications on their own system.
 *	This section sets the system that will be used by the current application.
 *	\note If the system number is changing, you will almost definitely need to restart the application.  
 *	restart, appExePath, appExeArgs are used to define if the application should restart now that
 *	the systemNumber has changed.  appExePath is the path to the application executable.  appExeArgs are
 *	the arguments that should be provided to run the executable (typically these will be the ones that
 *	were used to run the executable that is calling this function.
 */
void PortNums::setSystemNumber(QString appExePath,QStringList appExeArgs,int systemNumber,bool restart)
{
	if(systemNumber == systemNumber_)
		return;
	if(systemNumber > 10 || systemNumber < 1)
		return;
	QSettings settings((usedInSystemService_?QSettings::SystemScope:QSettings::UserScope),"Block Designs",appName_);
	settings.setValue("SystemNum",systemNumber);
	if(restart)
	{
		if(!QProcess::startDetached(appExePath,appExeArgs))
		{
			Q_ASSERT(false);
		}
		exit(0);
	}
	else
		updatePortNumbers();
}

/*! \brief Returns the current system number.*/
int PortNums::getSystemNumber()
{
	return systemNumber_;
}
	
/*! \brief Returns the current port used for downloading updated versions of the application.*/ 
int PortNums::getUpdatePort()
{
	return appUpdatePort_;
}
/*! \brief Returns the minimum port on which the PictoServer is allowed to communicate with client applications.*/
int PortNums::getMinDiscoverPort()
{
	return minDiscoverPort_;
}
/*! \brief Returns the maximum port on which the PictoServer is allowed to communicate with client applications.*/
int PortNums::getMaxDiscoverPort()
{
	return maxDiscoverPort_;
}

/*! \brief Returns the port on which the Picto Server listens for UDP DISCOVER datagrams.
*/
int PortNums::getServerPort()
{
	return serverPort_;
}
/*! \brief Returns the port on which command packets will be sent between the Director and the Pictobox FrontPanel.
*/
int PortNums::getLCDCommandPort()
{
	return lcdCommandPort_;
}
/*! \brief Returns the port on which event packets will be sent between the Director and the Pictobox FrontPanel.
*/
int PortNums::getLCDEventPort()
{
	return lcdEventPort_;
}
/*! \brief Updates the port numbers based on the current systemNumber_ value. 
*/
void PortNums::updatePortNumbers()
{
	QSettings settings((usedInSystemService_?QSettings::SystemScope:QSettings::UserScope),"Block Designs",appName_);
	if(!settings.contains("SystemNum"))
	{
		settings.setValue("SystemNum",1);
	}
	else
		systemNumber_ = settings.value("SystemNum").toInt();
	if(systemNumber_ > 10 || systemNumber_ < 1)
		systemNumber_ = 1;

	if(systemNumber_ > 1)
	{
		int referenceLoc = 52020+(100*systemNumber_);
		appUpdatePort_ = referenceLoc+3;
		minDiscoverPort_ = referenceLoc+5;
		maxDiscoverPort_ = minDiscoverPort_ + 75;
		serverPort_ = referenceLoc+4;
		lcdCommandPort_ = referenceLoc+2;
		lcdEventPort_ = referenceLoc+1;
	}
	else
	{
		//Before we set up the "System Number" system, the standard installation
		//was on ports with reference location of 42320.  Really, our ports should
		//be in the Dynamic/private/ephemeral ports range of 49152-65535, but in 
		//in order to support updating from the previous version, we needed to 
		//put system ports where they were before for system 1
		int referenceLoc = 42320+(100*systemNumber_);
		appUpdatePort_ = referenceLoc+3;
		minDiscoverPort_ = referenceLoc+5;
		maxDiscoverPort_ = minDiscoverPort_ + 75;
		serverPort_ = referenceLoc+4;
		lcdCommandPort_ = referenceLoc+2;
		lcdEventPort_ = referenceLoc+1;
	}
}

}; //namespace Picto
