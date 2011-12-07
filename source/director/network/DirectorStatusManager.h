#ifndef DIRECTORSTATUSMANAGER_H
#define DIRECTORSTATUSMANAGER_H

#include "../../common/network/ComponentStatusManager.h"
#include "../../common/engine/PictoEngine.h"
#include "../../common/experiment/experiment.h"

class DirectorStatusManager : public ComponentStatusManager
{
public:
	void setEngine(QSharedPointer<Picto::Engine::PictoEngine> engine);
	QSharedPointer<Picto::Engine::PictoEngine> getEngine();
	void setExperiment(QSharedPointer<Picto::Experiment> experiment);
	QSharedPointer<Picto::Experiment> getExperiment();
	void updateSplashStatus(QString status);
	virtual void setStatus(ComponentStatus status);
protected:
	virtual void newSession();
	virtual void doServerUpdate();
private:
	ComponentStatus status_;
	QWeakPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	static QMap<QString,ComponentStatus> statusTransitionDictionary_;
	static QMap<ComponentStatus,QString> statusNameDictionary_;
	int alignmentCode_;
	int alignmentID_;
	QDateTime lastAlignTime_;
};
#endif