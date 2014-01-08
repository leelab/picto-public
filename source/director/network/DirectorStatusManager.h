#ifndef DIRECTORSTATUSMANAGER_H
#define DIRECTORSTATUSMANAGER_H

#include "../../common/network/ComponentStatusManager.h"
#include "../../common/engine/PictoEngine.h"
#include "../../common/experiment/experiment.h"
#include "../../common/storage/directordata.h"

/*! \brief Handles overall status operations for a Director object.
 *	\details This class adds to the default ComponentStatusManager functionality by implimenting Status and user info display
 *	for the Experiment subject, interfacing with any Control Panels attached to the Director, sending Alignment Events to
 *	attached neural systems, and sending COMPONENTUPDATE commands and Alignment Event data to the Picto Server.
 *	\sa ComponentStatusManager
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class DirectorStatusManager : public ComponentStatusManager
{
	Q_OBJECT
public:
	virtual QString getName();
	void setEngine(QSharedPointer<Picto::Engine::PictoEngine> engine);
	void setDirectorData(QSharedPointer<Picto::DirectorData> directorData);
	QSharedPointer<Picto::Engine::PictoEngine> getEngine();
	void setExperiment(QSharedPointer<Picto::Experiment> experiment);
	QSharedPointer<Picto::Experiment> getExperiment();
	virtual void setUserInfo(QString info);
	virtual void setStatus(ComponentStatus status);
protected:
	virtual void newSession();
	virtual void doFrequentUpdate();
	virtual void doServerUpdate();
private:
	ComponentStatus status_;
	QWeakPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::DirectorData> directorData_;
	QSharedPointer<Picto::Experiment> experiment_;
	int alignmentCode_;
	int alignmentID_;
	QDateTime lastAlignTime_;
private slots:
	void pauseRequested();

};
#endif