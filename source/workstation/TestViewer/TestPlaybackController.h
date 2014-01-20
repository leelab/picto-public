#ifndef TESTTestPlaybackController_H_
#define TESTTestPlaybackController_H_
#include <QObject>
#include <QMutex>
#include <QSharedPointer>
#include <QThread>
#include "../../common/experiment/Experiment.h"

#include "../../common/engine/pictoengine.h"
using namespace Picto;

/*! \brief The TestPlaybackController handles the play(), pause(), stop() interface to an Experiment.
 *	\details This is essentially a convenience class that allows us to just start and stop running Tasks
 *	in an Experiment without worrying about details like its interaction with the Engine::PictoEngine.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class TestPlaybackController : public QObject
{
	Q_OBJECT
public:
	TestPlaybackController(QSharedPointer<Experiment> experiment);
	~TestPlaybackController();
	void setTask(QString taskName);
	bool isRunning();

public slots:
	void play();
	void pause();
	void stop();

signals:
	/*! \brief Emitted just before the Task starts running.*/
	void running();
	/*! \brief Emitted just before the Task pauses.*/
	void paused();
	/*! \brief Emitted after the Task stop running.*/
	void stopped();

private:
	QSharedPointer<Picto::Experiment> experiment_;
	QString taskName_;
	bool running_;
	bool paused_;
	bool stopping_;
};


#endif