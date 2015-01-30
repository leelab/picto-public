#include <QMutexLocker>
#include <QApplication>
#include <QThread>
#include "Scene.h"
#include "../memleakdetect.h"
#include "../stimuli/boxgraphic.h"
using namespace Picto;

/*! \brief Returns true if the layer of the first VisualElement input is lower than the layer of the second.*/
bool visualElementLessThan(const QSharedPointer<VisualElement> &e1, const QSharedPointer<VisualElement> &e2) 
{ 
	return e1->getLayer() < e2->getLayer(); 
}

float Scene::zoom_ = 1.0;
bool Scene::closeRenderLoops_ = false;
QMutex Scene::staticMutex_;

/*! \brief Creates a new Scene object and returns a shared pointer to it.
 *	\note The scene handles rendering to the UI so it's rendering system must run in the UI thread.
 *	To make sure this happens, we use the qt signal->slot system which gaurantees that if
 *	we mark this object as running the main thread, any slots that are triggered on the 
 *	object will wait for the UI thread to run.  Since we only call the private doRender()
 *	function through a signal->slot connection, we are gauranteed that rendering is in the
 *	UI thread.
 *	\sa render(), doRender(), readyForRender()
*/
QSharedPointer<Scene> Scene::createScene()
{
	QSharedPointer<Scene> result(new Scene());

	//Put the scene in the main UI thread.
	result.data()->moveToThread(QApplication::instance()->thread());
	return result;
}

/*! \brief Sets up rendering and then causes the actual render to happen by triggering the doRender() function.
 *	\details The actual rendering code that displays to the screen must run in the main UI thread
 *	but we don't want to require everything else to be in that thread.  Originally Picto
 *	wasn't designed with seperation of the View and Control logic into different threads
 *	so we needed to allow rendering code to occur in the UI thread and Control logic to
 *	occur in a separate thread without rewriting the whole system.  The method that we
 *	are using for now is to simply setup the render in this routine, then pause the Control logic
 *	thread until the doRender() which actually performs the render is called in the UI thread.  
 *	\note When running on the PictoDirector there is only one thread (ie. everything runs in the
 *	UI Thread) and so there is no pause.
 *
 *	callerId is the AssetId of the Asset calling render().
 */
void Scene::render(QSharedPointer<Engine::PictoEngine> engine,int callerId)
{
	//Setup rendering routine.
	QMutexLocker locker(mutex_.data());
	engine_ = engine;
	readyToRender_ = true;
	
	//Tell the render routine to run the next time the main UI thread has the processor.
	emit readyForRender(callerId);

	//Wait for the render routine to finish.
	bool endLoop = false;
	if(readyToRender_)
	{	//If we're in here, we're running with more the experiment in a non-ui thread.
		do
		{
			//Allow UI thread to do the render.  Unlock, yield this thread and relock.
			locker.unlock();
			QThread::currentThread()->yieldCurrentThread();
		
			staticMutex_.lock();
			endLoop = closeRenderLoops_;
			staticMutex_.unlock();
			locker.relock();
			//Check if the render occured, if not, loop back to "Allow UI thread to do the render"
		}while(readyToRender_ && !endLoop);
	}
}

/*! \brief Not currently used, but the idea is that this would set the background color rendered by this Scene.
*/
void Scene::setBackgroundColor(QColor color)
{
	QMutexLocker locker(mutex_.data());
	backgroundColor_ = color;
}

/*! \brief Adds the input VisualElement to this Scene so that it will be displayed next frame if it is marked visible.*/
void Scene::addVisualElement(QSharedPointer<VisualElement> element)
{
	QMutexLocker locker(mutex_.data());
	unaddedVisualElements_.push_back(element);
}

/*! \brief Adds the input AudioElement to this Scene so that it will be played next frame if it is set to play.*/
void Scene::addAudioElement(QSharedPointer<AudioElement> element)
{
	QMutexLocker locker(mutex_.data());
	unaddedAudioElements_.push_back(element);
}

/*! \brief Adds the input OutputSignal to this Scene so that its voltage effects will be triggered next frame.*/
void Scene::addOutputSignal(QSharedPointer<OutputSignal> element)
{
	QMutexLocker locker(mutex_.data());
	unaddedOutputSignals_.push_back(element);
}

/*! \brief Sets the current zoom level of this scene.
 *	\details The zoom value must be greater than zero and less than 1. Essentially, this value will
 *	multiply all of the dimensions of objects in the Scene to create smaller objects for smaller zoom values
 *	and simulate a 'Zoom Out' effect.
*/
void Scene::setZoom(float zoom)
{
	staticMutex_.lock();
	zoom_ = zoom;
	staticMutex_.unlock();
}

/*! \brief In case the render() function is stuck waiting for doRender() to render graphics for some reason, this function
 *	causes execution to break out of that loop.
 */
void Scene::closeRenderLoops()
{
	staticMutex_.lock();
	closeRenderLoops_ = true;
	staticMutex_.unlock();
}

/*! \brief Constructs a Scene object.
 *	\details Among other things, this connects the readyForRender() signal to the doRender() slot so that we can assure that all calls to 
 *	doRender() occur in the main UI thread.  See render(), doRender() for more details.
 */
Scene::Scene()
: backgroundColor_(QColor(Qt::black)),
  frame_(0)
{
	elapsedTime_.start();
	firstPhosphorTime_ = -1;
	readyToRender_ = false;
	mutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	connect(this,SIGNAL(readyForRender(int)),this,SLOT(doRender(int)));
}

/*! \brief Renders the Scene.
 *	\details This function sorts Visual Targets according to their layer, presents them to screen, plays any AudioElement objects that are currently
 *	audible, and pushes the latest output signals to their respective output ports.  callerId is the AssetId of the element that is starting this render()
 *	it will be used to report the active element during the course of each frame presentation.
 *	\note Qt requires that this function be called from the main UI thread since it calls functions that render graphics to screen.  This is handled by
 *	pushing this object into the UI thread and only calling doRender() from a signal-slot connection.  See render() for more details.
 */
void Scene::doRender(int callerId)
{
	//Scene must be called from the main thread.
	Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
	staticMutex_.lock();
	float frameZoom = zoom_;
	staticMutex_.unlock();

	QMutexLocker locker(mutex_.data());
	bool sceneRendered = false;
	do
	{
		//Grab the RenderingTargets from the engine
		QList<QSharedPointer< RenderingTarget> > renderingTargets;
		renderingTargets = engine_->getRenderingTargets();
		
		//Add any unadded visual elements to the visual elements list
		if(!unaddedVisualElements_.isEmpty())
		{
			QSharedPointer<Picto::CompositingSurface> compositingSurface;

			//add the appropriate compositing surfaces to the element
			for(int i=0; i<unaddedVisualElements_.length(); i++)
			{
				foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
				{
					compositingSurface = renderTarget->generateCompositingSurface();
					unaddedVisualElements_[i]->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
				}
				visualElements_.append(unaddedVisualElements_[i]);
			}
			unaddedVisualElements_.clear();
		}
		//Sort visual elements
		qSort(visualElements_.begin(), visualElements_.end(), &visualElementLessThan);

		//Add any unadded output signals to the outputSignals_ list
		if(!unaddedOutputSignals_.isEmpty())
		{
			//add the appropriate compositing surfaces to the element
			for(int i=0; i<unaddedOutputSignals_.length(); i++)
			{
				//Do Output Signal setup here
				outputSignals_.append(unaddedOutputSignals_[i]);
			}
			unaddedOutputSignals_.clear();
		}

		//Set Output Signals Values to engine
		bool enabled;
		int pinId;
		QVariant value;
		QString port;
		foreach(QSharedPointer<OutputSignal> outputSignal, outputSignals_)
		{
			port = outputSignal->getPort();
			pinId = outputSignal->getPin();
			enabled = outputSignal->getEnabled();
			value = outputSignal->getValue();
			engine_->enableOutputSignal(port,pinId,enabled);
			engine_->setOutputSignalValue(port,pinId,value);
		}






		//Add any unadded audio elements to the audio elements list
		if(!unaddedAudioElements_.isEmpty())
		{
			QSharedPointer<Picto::MixingSample> mixingSample;

			//add the appropriate compositing surfaces to the element
			for(int i=0; i<unaddedAudioElements_.length(); i++)
			{
				foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
				{
					mixingSample = renderTarget->generateMixingSample();
					unaddedAudioElements_[i]->addMixingSample(mixingSample);
				}
				audioElements_.append(unaddedAudioElements_[i]);
			}
			unaddedAudioElements_.clear();
		}

		//Render visual elements to each rendering target
		foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
		{
			QSharedPointer<VisualTarget> visualTarget = renderTarget->getVisualTarget();
			//Update zoom value for visual target.
			visualTarget->setZoom(frameZoom);

			//run through all our visualElements updating the animation and drawing
			foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
			{
				visualElement->updateAnimation(frame_,elapsedTime_);
				if(visualElement->getVisibleByUser(!engine_->operatorIsUser()))
					visualTarget->draw(visualElement->getPosition(),visualElement->getPositionOffset(),visualElement->getCompositingSurface(visualTarget->getTypeName()));
			}
			
			//Present it
			visualTarget->present();
		}

		//Render audio elements to each rendering target
		foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
		{
			QSharedPointer<AuralTarget> auralTarget = renderTarget->getAuralTarget();

			//run through all our audioElements and mix them into the auralTarget
			foreach(QSharedPointer<AudioElement> audioElement, audioElements_)
			{
				auralTarget->mix(audioElement->getMixingSample(auralTarget->getTypeName()));
			}
			
			//Present it
			auralTarget->present();
		}

		if(renderingTargets.size() && renderingTargets.first()->getVisualTarget())
		{
			firstPhosphorTime_ = renderingTargets.first()->getVisualTarget()->getLatestFirstPhosphor();
			sceneRendered = renderingTargets.first()->getVisualTarget()->latestFrameSuccesfullyRendered();
		}
	}while(!sceneRendered);
	frame_++;
	elapsedTime_.restart();
	//! firstPhosphorTime_ is the time that the first phosphor appeared on the first visual target
	//! In the current code iteration, visual targets block until the first phosphor of their
	//!	scene is drawn to the display.  This means that if there are multiple visual targets, things
	//! are not going to be synchronized.  In keeping with this deficiency, the recorded first phosphor
	//! time is simply the time that the first phosphor appeared on the first screen.  Down the line
	//! we should synchronize multiple displays.
	engine_->reportNewFrame(firstPhosphorTime_,callerId);
	readyToRender_ = false;
}