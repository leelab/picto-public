#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_

#include <QString>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "../common/network/ComponentInterface.h"
#include "../common/engine/PictoEngine.h"
#include "HardwareSetup.h"

class Director : public ComponentInterface
{
public:
	Director(QString name = "",
		HardwareSetup::SignalChannelType sigChannel = HardwareSetup::Mouse,
		HardwareSetup::VisualTargetType visualTarget = HardwareSetup::Pixmap,
		HardwareSetup::RewardControllerType rewardController = HardwareSetup::NullReward,
		HardwareSetup::EventCodeGeneratorType eventCodeGenerator = HardwareSetup::NullGen,
		int xEyeChannel = 0,
		int yEyeChannel = 1,
		int xDiamChannel = -1,
		int yDiamChannel = -1,
		int posSampPer = 2,
		int diamSampPer = 4);
	virtual ~Director();
protected:
	virtual QString name();
	virtual int openDevice();
	virtual int closeDevice();
	
private:
	QString name_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSqlDatabase configDb_;
	HardwareSetup::SignalChannelType sigChannel_;
	HardwareSetup::VisualTargetType visualTarget_;
	HardwareSetup::RewardControllerType rewardController_;
	HardwareSetup::EventCodeGeneratorType eventCodeGenerator_;
	int xChannel_;
	int yChannel_;
	int xDiamChannel_;
	int yDiamChannel_;
	int posSampPer_;
	int diamSampPer_;

};
#endif