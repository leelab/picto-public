#ifndef PICTOSYSTEMSTATE_H
#define PICTOSYSTEMSTATE_H

#include <QSharedPointer>
#include <QVector>
#include <QMap>
#include <QString>
#include "PictoDeviceSimulatorTypes.h"
namespace PictoSim{

/*! \brief Defines a single activity report.
 *	Includes a failure message, an expected value, and an actual value that didn't match up to the expected value.
 */
struct PictoSystemActivityReport
{
	PictoSystemActivityReport(){};
	PictoSystemActivityReport(QString failMessage, QString actualVal, QString expectedVal);
	QString failMessage_;
	QString actualVal_;
	QString expectedVal_;
};

class PictoDeviceSimulator;
/*! \brief This class identifies the current state of the simulated picto system.
 *	Various functions are included for generating new PictoDeviceSimulators, accessing 
 *	them and removing them.  All PictoDeviceSimulators hold a pointer to this object
 *	giving them access to the state of the full PictoSystem for the purpose of accurate
 *	error checking.
 */
class PictoSystemState
{
public:
	void AssureDeviceExistance(QString name, SimActorType devType);
	void RemoveDevice(QString name);
	QSharedPointer<PictoDeviceSimulator> GetDevice(QString name);
	unsigned int NumDevices();
	QMap<QString, QSharedPointer<PictoDeviceSimulator>>& GetDeviceMap();
	QString GetServerName(){return serverName_;};

	void PushActivityReport(PictoSystemActivityReport& activityReport);
	bool PopActivityReport(PictoSystemActivityReport& activityReport);
private:
	QMap<QString, QSharedPointer<PictoDeviceSimulator>> devices_;	// The map of PictoDeviceSimulators indexed by their name.
	QVector<PictoSystemActivityReport> activityReports_;			// A list of pending activity reports.
	QString serverName_;
};

}; // PictoSim namespace

#endif