#include "Scene.h"
#include "../memleakdetect.h"
#include "../stimuli/boxgraphic.h"
using namespace Picto;

bool visualElementLessThan(const QSharedPointer<VisualElement> &e1, const QSharedPointer<VisualElement> &e2) 
{ 
	return e1->getLayer() < e2->getLayer(); 
}

Scene::Scene()
: backgroundColor_(QColor(Qt::black)),
  frame_(0)
{
	elapsedTime_.start();
	firstPhosphorTime_ = -1;
}

void Scene::render(QSharedPointer<Engine::PictoEngine> engine)
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

	//Render visual elements to each rendering target
	foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
	{
		QSharedPointer<VisualTarget> visualTarget = renderTarget->getVisualTarget();
		
		//run through all our visualElements updating the animation and drawing
		foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
		{
			visualElement->updateAnimation(frame_,elapsedTime_);
			if(visualElement->getVisibleByUser(!engine->operatorIsUser()))
				visualTarget->draw(visualElement->getPosition()-visualElement->getPositionOffset(),visualElement->getCompositingSurface(visualTarget->getTypeName()));
		}
		
		//Present it
		visualTarget->present();
	}
	if(renderingTargets.size() && renderingTargets.first()->getVisualTarget())
		firstPhosphorTime_ = renderingTargets.first()->getVisualTarget()->getLatestFirstPhosphor();
	frame_++;
	elapsedTime_.restart();
}

//! Gets the time that the first phosphor appeared on the first visual target
//! In the current code iteration, visual targets block until the first phosphor of their
//!	scene is drawn to the display.  This means that if there are multiple visual targets, things
//! are not going to be synchronized.  In keeping with this deficiency, the recorded first phosphor
//! time is simply the time that the first phosphor appeared on the first screen.  Down the line
//! we should synchronize multiple displays.
double Scene::getLatestFirstPhosphorTime()
{
	return firstPhosphorTime_;
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
