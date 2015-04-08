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
 *	\details A scene is the thing that actually puts images on the screen, plays audio, and sets voltages to output pins.  Scenes 
 *	are used by the two StateMachineElements in which experimental time passes, State and PausePoint.  Each of these contains a single Scene.
 *	To use a scene, VisualElements, AudioElements, and OutputSignals are added using their respective add?() functions.  When it comes time
 *	to render the elements for the test subject, render() is called with the PictoEngine and the AssetId of the element calling the function.
 *
 *	\note There is some complexity associated with this object due to the fact that Qt requires that all rendering activities occur in the
 *	main UI thread.  Since we want to be able to run Experiments in a separate thread, we needed to set up a system in which we can be sure
 *	that the render code gets run in the main UI thread, but everything else can run in a separate thread.  We handle this by moving the
 *	Scene object into the UI thread, and then calling its private render code using a signal-slot framework.  This is all handled internally
 *	and needn't concern outside classes.  For more detail on this, see render() and doRender().
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
	 *	only in the main UI thread.  See renderI(), doRender() for more detail.
	 */
	void readyForRender(int callerId);


private:
	Scene();
	QSharedPointer<QMutex> mutex_;
	QSharedPointer<Engine::PictoEngine> engine_;
	QColor backgroundColor_;
	QList<QSharedPointer <VisualElement> > visualElements_;
	QList<QSharedPointer <VisualElement> > unaddedVisualElements_;

	QList<QSharedPointer <DataViewElement> > dataViewElements_;
	QList<QSharedPointer <DataViewElement> > unaddedDataViewElements_;

	QList<QSharedPointer <AudioElement> > audioElements_;
	QList<QSharedPointer <AudioElement> > unaddedAudioElements_;

	QList<QSharedPointer <OutputSignal> > outputSignals_;
	QList<QSharedPointer <OutputSignal> > unaddedOutputSignals_;

	QList<QSharedPointer <InputSignal> > inputSignals_;
	QList<QSharedPointer <InputSignal> > unaddedInputSignals_;
	
	int frame_;
	QTime elapsedTime_;
	double firstPhosphorTime_;
	bool readyToRender_;
	static float zoom_;
	static bool closeRenderLoops_;
	static QMutex staticMutex_;

private slots:
	void doRender(int callerId);
};


}; //namespace Picto

#endif
