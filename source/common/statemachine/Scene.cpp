#include "Scene.h"
#include "../memleakdetect.h"
#include "../stimuli/boxgraphic.h"
using namespace Picto;

bool visualElementLessThan(const QSharedPointer<VisualElement> &e1, const QSharedPointer<VisualElement> &e2) 
{ 
	return e1->getLayer() < e2->getLayer(); 
}

float Scene::zoom_ = 1.0;

Scene::Scene()
: backgroundColor_(QColor(Qt::black)),
  frame_(0)
{
	elapsedTime_.start();
	firstPhosphorTime_ = -1;
}

void Scene::render(QSharedPointer<Engine::PictoEngine> engine,int callerId)
{
	bool sceneRendered = false;
	do
	{
		//Grab the RenderingTargets from the engine
		QList<QSharedPointer< RenderingTarget> > renderingTargets;
		renderingTargets = engine->getRenderingTargets();
		
		//! \TODO "render" the audio stuff
		//! \todo deal with the background layer color
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
			engine->enableOutputSignal(port,pinId,enabled);
			engine->setOutputSignalValue(port,pinId,value);
		}

		//Render visual elements to each rendering target
		float frameZoom = zoom_;
		foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
		{
			QSharedPointer<VisualTarget> visualTarget = renderTarget->getVisualTarget();
			//Update zoom value for visual target.
			visualTarget->setZoom(frameZoom);

			//run through all our visualElements updating the animation and drawing
			foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
			{
				visualElement->updateAnimation(frame_,elapsedTime_);
				if(visualElement->getVisibleByUser(!engine->operatorIsUser()))
					visualTarget->draw(visualElement->getPosition(),visualElement->getPositionOffset(),visualElement->getCompositingSurface(visualTarget->getTypeName()));
			}
			
			//Present it
			visualTarget->present();
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
	engine->reportNewFrame(firstPhosphorTime_,callerId);
}

//! Resets the scene
void Scene::reset()
{
	
	foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
	{
		visualElement->reset();
	}

	foreach(QSharedPointer<VisualElement> visualElement, unaddedVisualElements_)
	{
		visualElement->reset();
	}

	foreach(QSharedPointer<OutputSignal> outputSignal, outputSignals_)
	{
		outputSignal->reset();
	}

	foreach(QSharedPointer<OutputSignal> outputSignal, unaddedOutputSignals_)
	{
		outputSignal->reset();
	}

}
void Scene::setBackgroundColor(QColor color)
{
	backgroundColor_ = color;
}

void Scene::addVisualElement(QSharedPointer<VisualElement> element)
{
	unaddedVisualElements_.push_back(element);
}

void Scene::addAudioElement(QSharedPointer<AudioElement> element)
{
	unaddedAudioElements_.push_back(element);
}

void Scene::addOutputSignal(QSharedPointer<OutputSignal> element)
{
	unaddedOutputSignals_.push_back(element);
}
