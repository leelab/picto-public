#include <QSettings>
#include <QProcess>
#include "portdefs.h"
#include "memleakdetect.h"

namespace Picto {

//When the Picto Server runs as a picto service, it does not have access to the regular Picto settings
//so they need to be saved in the System Scope.  All other Picto applications should save their values
//in the user scope (because setting system settings requires administrator previleges).
PortNums::PortNums(QString appName,bool usedInSystemService)
:
usedInSystemService_(usedInSystemService)
{
	appName_ = appName;
	systemNumber_ = 1;
	updatePortNumbers();
}

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

int PortNums::getSystemNumber()
{
	return systemNumber_;
}
	
int PortNums::getUpdatePort()
{
	return appUpdatePort_;
}
int PortNums::getMinDiscoverPort()
{
	return minDiscoverPort_;
}
int PortNums::getMaxDiscoverPort()
{
	return maxDiscoverPort_;
}
int PortNums::getServerPort()
{
	return serverPort_;
}
int PortNums::getLCDCommandPort()
{
	return lcdCommandPort_;
}
int PortNums::getLCDEventPort()
{
	return lcdEventPort_;
}

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
