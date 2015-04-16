#ifndef _SCENE_H_
#define _SCENE_H_

#include <QObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QMutex>

#include "../common.h"
#include "../stimuli/AudioElement.h"
#include "../controlelements/ControlElement.h"
#include "../compositor/RenderingTarget.h"
#include "../engine/PictoEngine.h"
#include "../stimuli/visualelement.h"
#include "../stimuli/outputsignal.h"
#include "../stimuli/inputsignal.h"

namespace Picto {

/*!	\brief Handles rendering of OutputElements each frame.
 *
 *	\details A scene is the thing that actually puts images on the screen, plays audio, and sets voltages to output pins.
 *	Scenes are used by the two StateMachineElements in which experimental time passes, State and PausePoint.  Each of
 *	these contains a single Scene.  To use a scene, VisualElements, AudioElements, and OutputSignals are added using their
 *	respective add?() functions.  When it comes time to render the elements for the test subject, render() is called with
 *	the PictoEngine and the AssetId of the element calling the function.
 *
 *	\note There is some complexity associated with this object due to the fact that Qt requires that all rendering
 *	activities occur in the main UI thread.  Since we want to be able to run Experiments in a separate thread, we needed
 *	to set up a system in which we can be sure that the render code gets run in the main UI thread, but everything else
 *	can run in a separate thread.  We handle this by moving the Scene object into the UI thread, and then calling its
 *	private render code using a signal-slot framework.  This is all handled internally and needn't concern outside classes.
 *	\sa Scene::render, Scene::doRender
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Scene : public QObject
#else
class Scene : public QObject
#endif
{
	Q_OBJECT
public:

	static QSharedPointer<Scene> createScene();

	void render(QSharedPointer<Engine::PictoEngine> engine,int callerId);
	void setBackgroundColor(QColor color);
	void addVisualElement(QSharedPointer<VisualElement> element);
	void addDataViewElement(QSharedPointer<DataViewElement> element);
	void addAudioElement(QSharedPointer<AudioElement> element);
	void addOutputSignal(QSharedPointer<OutputSignal> element);
	void addInputSignal(QSharedPointer<InputSignal> element);
	static void setZoom(float zoom);
	static void closeRenderLoops();

signals:
	/*! \brief Emitted when rendering is setup and the object wants doRender() to be called.
	 *	\details This signal essentially exists in order to deal with a Qt requirement that all rendering activity occurs
	 *	only in the main UI thread.
	 *	\sa Scene::render(), Scene::doRender()
	 */
	void readyForRender(int callerId);


private:
	Scene();
	//! Mutex used to lock lists during rendering
	QSharedPointer<QMutex> mutex_;
	//! A sharedpointer to the PictoEngine controlling the logic
	QSharedPointer<Engine::PictoEngine> engine_;
	//! The color of the background - We do not currently modify this
	QColor backgroundColor_;

	//! A QList of VisualElements that we are updating.
	QList<QSharedPointer <VisualElement> > visualElements_;
	//! A QList of VisualElements that we stream in from the Task.
	QList<QSharedPointer <VisualElement> > unaddedVisualElements_;

	//! A QList of DataViewElements that we are updating.
	QList<QSharedPointer <DataViewElement> > dataViewElements_;
	//! A QList of DataViewElements that we stream in from the Task.
	QList<QSharedPointer <DataViewElement> > unaddedDataViewElements_;

	//! A QList of AudioElements that we are updating.
	QList<QSharedPointer <AudioElement> > audioElements_;
	//! A QList of AudioElements that we stream in from the Task.
	QList<QSharedPointer <AudioElement> > unaddedAudioElements_;

	//! A QList of OutputSignals that we are updating.
	QList<QSharedPointer <OutputSignal> > outputSignals_;
	//! A QList of OutputSignals that we stream in from the Task.
	QList<QSharedPointer <OutputSignal> > unaddedOutputSignals_;

	//! A QList of InputSignals that we are updating.
	QList<QSharedPointer <InputSignal> > inputSignals_;
	//! A QList of InputSignals that we stream in from the Task.
	QList<QSharedPointer <InputSignal> > unaddedInputSignals_;
	
	//! Frame count since start of Scene
	int frame_;
	//! Elapsed time since start of Scene
	QTime elapsedTime_;

	/*! \brief The time that the first phosphor appeared on the first visual target.
	 *	\note In the current code iteration, visual targets block until the first phosphor of their scene is drawn to
	 *	the display.  This means that if there are multiple visual targets, things are not going to be synchronized.
	 *	In keeping with this deficiency, the recorded first phosphor time is simply the time that the first phosphor
	 *	appeared on the first screen.  Down the line we should synchronize multiple displays.
	 */
	double firstPhosphorTime_;
	//! Presumably to protect against spurious wakeup of rendering thread
	bool readyToRender_;
	//! Current level of zoom
	static float zoom_;
	//! Escape flag used to stop rendering when we're trying to quit
	static bool closeRenderLoops_;
	//! Mutex used to sync closeRenderLoops_
	static QMutex staticMutex_;

private slots:
	void doRender(int callerId);
};


}; //namespace Picto

#endif
