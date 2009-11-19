#include "VisualElement.h"

namespace Picto {

VisualElement::VisualElement() :
	shouldRedrawImage_(true)
{

}

void VisualElement::addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface)
{
	compositingSurface->convertImage(image_);
	compositingSurfaces_[surfaceType] = compositingSurface;
}

void VisualElement::updateCompositingSurfaces()
{
	foreach(QSharedPointer<CompositingSurface> compositingSurface, compositingSurfaces_.values())
	{
		compositingSurface->convertImage(image_);
	}
}

}; //namespace Picto
