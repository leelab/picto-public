#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_
#include <QString>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "../common/network/ComponentInterface.h"
#include "../common/engine/PictoEngine.h"
#include "../common/experiment/experiment.h"
#include "HardwareSetup.h"

class Director : public ComponentInterface
{
public:
	Director(QString name = "",
		HardwareSetup::SignalChannelType sigChannel = HardwareSetup::Mouse,
		HardwareSetup::VisualTargetType visualTarget = HardwareSetup::Pixmap,
		HardwareSetup::RewardControllerType rewardController = HardwareSetup::NullReward,
		HardwareSetup::EventCodeGeneratorType eventCodeGenerator = HardwareSetup::NullGen);
	~Director();
protected:
	virtual QString componentType();
	virtual QString name();
	virtual bool continueRunning();
	virtual int openDevice();
	virtual int closeDevice();
	virtual int startSession(QUuid sessionID);
	virtual int loadExp(QString expXML);
	virtual int startExp(QString taskName);
	virtual int reward(int channel);
	virtual int reportServerConnectivity(bool connected);
	virtual int reportErrorDirective();
	virtual int reportUnsupportedDirective(QString directive);
	virtual int endSession();
	
private:
	void updateSplashStatus(QString status);
	bool EngineHasRenderingTargets;
	QString name_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSqlDatabase configDb_;
	HardwareSetup::SignalChannelType sigChannel_;
	HardwareSetup::VisualTargetType visualTarget_;
	HardwareSetup::RewardControllerType rewardController_;
	HardwareSetup::EventCodeGeneratorType eventCodeGenerator_;

};
#endif