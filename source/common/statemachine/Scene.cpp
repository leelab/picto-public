#include "Scene.h"
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
}

void Scene::render(QSharedPointer<Engine::PictoEngine> engine)
{
	//! \TODO "render" the audio stuff
	//! \todo deal with the background layer color

	//Grab the RenderingTargets from the engine
	QList<QSharedPointer< RenderingTarget> > renderingTargets;
	renderingTargets = engine->getRenderingTargets();

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
			if(visualElement->getVisibleByUser(!engine->slaveMode()))
				visualTarget->draw(visualElement->getPosition(),visualElement->getCompositingSurface(visualTarget->getTypeName()));
		}
		
		//Present it
		visualTarget->present();
	}
	frame_++;
	elapsedTime_.restart();
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
