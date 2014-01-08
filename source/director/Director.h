#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_

#include <QString>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QProcess>
#include "../common/network/ComponentInterface.h"
#include "../common/engine/PictoEngine.h"
#include "HardwareSetup.h"
#include "frontpanel/FPInterface.h"
#include "../common/storage/directordata.h"

/*! \brief Defines an object that runs a Picto Experiment for a test subject, transmits experimental data to the
 *	Picto server and sends timing alignment events to a Neural data acquisition system.
 *	\details This class implements a ComponentInterface to setup a DirectorStatusManager, a Picto::Engine::PictoEngine and
 *	a CommandChannel that work together to run Experiments that come in from the PictoServer.  When running on a Pictobox it 
 *	also starts an instance of the EmbeddedFrontPanel process which functions as a simple control panel for things like 
 *	supplying rewards and displaying feedback to the Experiment operator.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class Director : public QObject, public ComponentInterface
{
	Q_OBJECT
public:
	Director(QString name = "",
		HardwareSetup::SignalChannelType sigChannel = HardwareSetup::Mouse,
		HardwareSetup::VisualTargetType visualTarget = HardwareSetup::Pixmap,
		HardwareSetup::RewardControllerType rewardController = HardwareSetup::NullReward,
		HardwareSetup::OutputSignalControllerType outSigController = HardwareSetup::NullOutSig,
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
	QSharedPointer<FPInterface> fpInterface_;
	QSharedPointer<Picto::DirectorData> directorData_;
	QSqlDatabase configDb_;
	HardwareSetup::SignalChannelType sigChannel_;
	HardwareSetup::VisualTargetType visualTarget_;
	HardwareSetup::RewardControllerType rewardController_;
	HardwareSetup::OutputSignalControllerType outSigController_;
	HardwareSetup::EventCodeGeneratorType eventCodeGenerator_;
	QStringList rewardDurs_;
	QStringList flushDurs_;
	QSharedPointer<QProcess> frontPanelProcess_;
	int xChannel_;
	int yChannel_;
	int xDiamChannel_;
	int yDiamChannel_;
	int posSampPer_;
	int diamSampPer_;
	bool useFrontPanel_;
private slots:
	void changeName(QString name);
	void changeRewardDuration(int controller, int duration);
	void changeFlushDuration(int controller, int duration);

};
#endif