#ifndef PICTOSYSTEMSTATE_H
#define PICTOSYSTEMSTATE_H

#include <QSharedPointer>
#include <QVector>
#include <QMap>
#include <QString>
#include "PictoDeviceSimulatorTypes.h"
namespace PictoSim{

struct PictoSystemActivityReport
{
	PictoSystemActivityReport(){};
	PictoSystemActivityReport(QString failMessage, QString actualVal, QString expectedVal);
	QString failMessage_;
	QString actualVal_;
	QString expectedVal_;
};

/*! \brief 
 */
class PictoDeviceSimulator;
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
	QMap<QString, QSharedPointer<PictoDeviceSimulator>> devices_;
	QVector<PictoSystemActivityReport> activityReports_;
	QString serverName_;
};

}; // PictoSim namespace

#endif