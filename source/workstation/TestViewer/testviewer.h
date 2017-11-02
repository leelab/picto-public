#ifndef _TEST_VIEWER_H_
#define _TEST_VIEWER_H_

#include "../viewer.h"
#include "TestPlaybackController.h"
#include "../../common/Designer/AnalysisSelectorWidget.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/iodevices/VirtualOutputSignalController.h"
#include "../ReplayViewer/OutputWidgetHolder.h"
#include "../../common/playback/LiveFrameReader.h"
#include "../../common/playback/LiveLfpReader.h"
#include "../../common/playback/LiveRewardReader.h"
#include "../../common/playback/LiveRunNotesReader.h"
#include "../../common/playback/LiveSpikeReader.h"
#include "../../common/playback/LiveSignalReader.h"
#include "../ReplayViewer/RecordingVisualTargetHost.h"

QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QComboBox;
class QSlider;
QT_END_NAMESPACE

class ViewSelectionFrame;
class QLCDNumber;

/*!	\brief	This allows designers to run the Experiment/Analysis that they are creating in a test
 *	environment for debugging.
 *
 *	\details This Viewer runs the Experiment inside a widget, using a Pixmap rendering target, 
 *	and a mouse input channel.  Analyses can be run during the course of the Experiment and should
 *	provide the same results as they would in real experiments with a couple of exceptions:
 *		- AnalysisDataSource elements will not have access to future data (since it doesn't exist yet), 
 *			depending on the type of future data request, the result will either be meaningless or empty.
 *		- Some AnalysisDataSource data will be simulated.  LFP data for example is fake since there is
 *			no "Test Neural System" attached.
 *		- Frame rates may be much faster than they would be in a real experiment.  This is because the
 *			PixmapVisualTarget doesn't know how to wait for VSync while rendering, so it just renders
 *			and continues on.  This could cause the TestViewer to think that many frames have gone
 *			by even though there may only have been one.
 *
 *	The toolbar for the Test Viewer includes start/pause/stop commands as well as a drop down for choosing
 *	the Task to run, another drop down to select the "viewer" as either a Test Subject or an Operator.  There
 *	is also a PropertyFrame for changing Task Property InitValues during the run, an AnalysisSelectorWidget
 *	for choosing Analyses to run during the Test, and an OutputWidgetHolder for presenting
 *	AnalysisOutputWidgets
 *
 *	This Test Viewer also contains a simple debugger which allows the designer to check variable values and
 *	even run javascript in a command line at the bottom of the window.  The debugger is the stock Qt debugger 
 *	and is used to debug Picto scripts.  It is set up in ScriptableContainer::initScripting().  The debugger 
 *	stops Experiment execution and waits for the user to press its play button before continuing for one of 
 *	two reasons.
 *		- A runtime error occurs.
 *		- The keyword - debugger - appeared in a script.
 *	Since Picto currently creates one script engine for every ScriptableContainer, a different debugger window
 *	appears for every runtime error that occurs in a separate ScriptableContainer.  This can get unwieldy 
 *	quickly, and we should consider fixing this at some point.
 *
 *	\note Like in real Experimental Runs, pressing the pause button causes the Run to pause only when control
 *	flow reaches the next PausePoint element.
 *	Suggestions for future features:
 *		- Add a step function for advancing frame by frame
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
 */
class TestViewer : public Viewer
{
	Q_OBJECT
public:
	TestViewer(QWidget *parent=0);
	virtual ~TestViewer(){};
	QString type() { return "Test"; };

public slots:
	void init();
	void deinit();
	bool aboutToQuit();

	void LoadPropValsFromFile();

	//Neural data sonification
	void spikeAdded(double time, int channel, int unit, QVariantList waveform);
	void rewarded(int quantity);

private:
	void setupEngine();
	void setupUi();
	void generateComboBox();

	void updateNeuralUI();
	void updateRecordingTarget();

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<TestPlaybackController> testController_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;

	//! Holds and renders the Task view
	RecordingVisualTargetHost  *visualTargetHost_;
	QWidget *propertyFrame_;
	Picto::AnalysisSelectorWidget* analysisSelector_;
	QVector<QWidget *> outputSignalsWidgets_;

	QAction *playAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	QAction *loadPropsAction_;

	QToolBar* testToolbar_;

	QComboBox *taskListBox_;
	QComboBox *userType_;
	OutputWidgetHolder* outputWidgetHolder_;
	//! The widget that can rearrange the ViewWidgets in the Central View
	ViewSelectionFrame *viewSelectionFrame_;

	bool deiniting_;
	enum Status {Ending, Stopped, Running, Paused};
	Status status_;

	QSharedPointer<LiveFrameReader> liveFrameReader_;
	QSharedPointer<LiveLfpReader> liveLfpReader_;
	QSharedPointer<LiveRewardReader> liveRewardReader_;
	QSharedPointer<LiveSpikeReader> liveSpikeReader_;
	QSharedPointer<LiveRunNotesReader> liveRunNotesReader_;
	QVector<QSharedPointer<LiveSignalReader>> signalReaders_;

	QComboBox* channelBox_;
	QComboBox* unitBox_;
	QSharedPointer<TaskConfig> currentTaskConfig_;
	QMap<int, QList<int>> ChannelsUnits_;
	bool noCallBack_;
	int selectedChannel_;
	int selectedUnit_;
	QSound NeuralTick_;
	QAction *toggleRecord_;
	QLCDNumber *recordTime_;
	QAction *restartRecord_;
	QAction *saveRecording_;
	bool recordModeOn_;
	bool isRunning_;

	QMap<int, QList<int>> alignElements_;
	QSharedPointer<DesignRoot> designRootForData_;
	int taskTabIndex_;

private slots:
	void playTriggered();
	void running();
	void paused();
	void stopped();
	void taskListIndexChanged(int index);
	void operatorClickDetected(QPoint pos);
	void setUserType(int index);
	void runStarted(QUuid runId);

	void toggleRecording();
	void restartRecording();
	void saveRecording();
	void setRecordTime(double time);
	void selectedChannelChanged(int channel);
	void selectedUnitChanged(int unit);

};

#endif