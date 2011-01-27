#include "PictoSystemState.h"
#include "PictoWorkstationSimulator.h"
#include "PictoDirectorSimulator.h"
#include "PictoProxyServerSimulator.h"
#include "PictoServerSimulator.h"
using namespace PictoSim;

/*! \brief Constructs a PictoSystemActivityReport with a failure message an expected value and a non matching actual value.
 */
PictoSystemActivityReport::PictoSystemActivityReport(QString failMessage, QString actualVal, QString expectedVal):
failMessage_(failMessage),
actualVal_(actualVal),
expectedVal_(expectedVal)
{}

/*! \brief Assures that a device of the input name and type exists, and if it doesn't, creates it.
 *	Note that the PictoTestBench at this time doesn't allow for two devices of the same name even
 *	if they are of different type.
 */
void PictoSystemState::AssureDeviceExistance(QString name, SimActorType devType)
{
	if(devType == SERVER)
	{	// Only ever create one server.
		if(GetDevice(serverName_).isNull() || (serverName_ == name))
		{
			serverName_ = name;
		}
		else
		{
			QFAIL("TEST DEFINITION ERROR: Multiple servers have been added to the Picto System but only one is allowed."); 
		}
	}
	// If the device hasn't been created yet, create it.
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

/*! \brief Removes the device with the input name from the list of devices.
 *	Currently, the device isn't necessarily destroyed, and action isn't stopped on 
 *	the device by this function.  It might make sense to change this.
 */
void PictoSystemState::RemoveDevice(QString name)
{
	devices_.remove(name);
}

/*! \brief Returns a shared pointer to the device with the input name.
 */
QSharedPointer<PictoDeviceSimulator> PictoSystemState::GetDevice(QString name)
{
	return devices_[name];
}

/*! \brief Returns a reference to map of devices.
 */
QMap<QString, QSharedPointer<PictoDeviceSimulator>>& PictoSystemState::GetDeviceMap()
{
	return devices_;
}

/*! \brief Adds an activity report to the system state.
 */
void PictoSystemState::PushActivityReport(PictoSystemActivityReport& activityReport)
{
	activityReports_.push_back(activityReport);
}

/*! \brief Removes an activity report from the system state, returning it through the input reference.
 */
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