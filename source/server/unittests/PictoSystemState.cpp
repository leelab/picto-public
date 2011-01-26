#include "PictoSystemState.h"
#include "PictoWorkstationSimulator.h"
#include "PictoDirectorSimulator.h"
#include "PictoProxyServerSimulator.h"
#include "PictoServerSimulator.h"
using namespace PictoSim;

PictoSystemActivityReport::PictoSystemActivityReport(QString failMessage, QString actualVal, QString expectedVal):
failMessage_(failMessage),
actualVal_(actualVal),
expectedVal_(expectedVal)
{}


void PictoSystemState::AssureDeviceExistance(QString name, SimActorType devType)
{
	if(devType == SERVER)
	{
		if(GetDevice(serverName_).isNull() || (serverName_ == name))
		{
			serverName_ = name;
		}
		else
		{
			QFAIL("TEST DEFINITION ERROR: Multiple servers have been added to the Picto System but only one is allowed."); 
		}
	}
	if(!devices_.contains(name))
	{
		switch(devType)
		{
		case WORKSTATION:
			devices_.insert(name, QSharedPointer<PictoDeviceSimulator>(new PictoWorkstationSimulator(this,name)) );
			break;
		case DIRECTOR:
			devices_.insert(name, QSharedPointer<PictoDeviceSimulator>(new PictoDirectorSimulator(this,name)) );
			break;
		case PROXYSERVER:
			devices_.insert(name, QSharedPointer<PictoDeviceSimulator>(new PictoProxyServerSimulator(this,name)) );
			break;
		case SERVER:
			devices_.insert(name, QSharedPointer<PictoDeviceSimulator>(new PictoServerSimulator(this,name)) );
			break;
		}
	}
}
void PictoSystemState::RemoveDevice(QString name)
{
	devices_.remove(name);
}
QSharedPointer<PictoDeviceSimulator> PictoSystemState::GetDevice(QString name)
{
	return devices_[name];
}

QMap<QString, QSharedPointer<PictoDeviceSimulator>>& PictoSystemState::GetDeviceMap()
{
	return devices_;
}

void PictoSystemState::PushActivityReport(PictoSystemActivityReport& activityReport)
{
	activityReports_.push_back(activityReport);
}
bool PictoSystemState::PopActivityReport(PictoSystemActivityReport& activityReport)
{	
	if(activityReports_.size()) 
	{
		activityReport = activityReports_.front();
		activityReports_.pop_front();
		return true;
	}
	return false;

};