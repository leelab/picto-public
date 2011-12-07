#ifndef _TEST_VIEWER_H_
#define _TEST_VIEWER_H_

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"

class QAction;
class QToolBar;
class QComboBox;

/*!	\brief	This views the experiment as a running test
 *
 *	Designers will want to test the experiments they have written.  This
 *	viewer allows them to do this.  It basically creates an engine inside
 *	a widget, using a Pixmap rendering target, and a mouse input channel.
 *
 *	The toolbar for this will include start/pause/stop commands
 *
 *	This is NOT a debugger.  A debugger would require a way to identify the 
 *	current state, pause/restart controls, and a JavaScript debugger.  This
 *	has none of that.
 *
 *	Suggestions for future features:
 *		- Pause should stop any timed controllers
 *		- Add a step function for advancing frame by frame
 */
class TestViewer : public Viewer
{
	Q_OBJECT
public:
	TestViewer(QWidget *parent=0);
	virtual ~TestViewer(){};
	QString type() { return "Test"; };

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer

	void play();
	void pause();
	void stop();
	void LoadPropValsFromFile();


private:
	void setupEngine();
	void setupUi();
	void generateComboBox();
	void resetExperiment();

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;

	Picto::VisualTargetHost *visualTargetHost_;
	QWidget *propertyFrame_;

	QAction *playAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	QAction *loadPropsAction_;

	QToolBar* testToolbar_;

	QComboBox *taskListBox_;

	enum Status {Ending, Stopped, Running, Paused};
	Status status_;
private slots:
	void taskListIndexChanged(int index);
	void operatorClickDetected(QPoint pos);
};

#endif