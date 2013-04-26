#ifndef _SLAVEEXPERIMENTDRIVER_H_
#define _SLAVEEXPERIMENTDRIVER_H_
#include <QSharedPointer>
#include <QTime>
#include "../experiment/experiment.h"
#include "StateUpdater.h"
#include "propertytable.h"

#include "../common.h"

namespace Picto {

/*! SlaveExperimentDriver
 *
 *	Connects a StateUpdater to an Experiment to drive an Experiment in slave mode.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API SlaveExperimentDriver : public QObject
#else
class SlaveExperimentDriver : public QObject
#endif
{
	Q_OBJECT
public:
	SlaveExperimentDriver(QSharedPointer<Experiment> exp,QSharedPointer<StateUpdater> updater);
	virtual ~SlaveExperimentDriver(){};
private:
	void renderFrame();
	QSharedPointer<Experiment> experiment_;
	QSharedPointer<StateUpdater> updater_;
	QSharedPointer<DesignConfig> designConfig_;
	QSharedPointer<StateMachineElement> currElement_;
	QTime frameTimer_;
	bool renderingEnabled_;
private slots:
	void masterPropertyValueChanged(int propId, QString value);
	void masterPropertyInitValueChanged(int propId, QString value);
	void masterTransitionActivated(int transId);
	void masterFramePresented(double time);
	void masterRewardSupplied(double time,int duration,int channel);
	void masterSignalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void disableRendering(bool disable);
};


}; //namespace Picto

#endif
