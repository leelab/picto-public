#ifndef TESTTestPlaybackController_H_
#define TESTTestPlaybackController_H_
#include <QObject>
#include <QMutex>
#include <QSharedPointer>
#include <QThread>
#include "../../common/experiment/Experiment.h"

#include "../../common/engine/pictoengine.h"
using namespace Picto;

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
	void running();
	void paused();
	void stopped();

private:
	QMutex mutex_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<QThread> controllerThread_;
	QString taskName_;
	bool running_;
	bool paused_;
	bool stopping_;
};


#endif