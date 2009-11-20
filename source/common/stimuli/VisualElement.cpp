#include "VisualElement.h"

namespace Picto {

VisualElement::VisualElement() :
	shouldRedrawImage_(true),
	parameterContainer_("Visual Element")
{
}

QPoint VisualElement::getPosition()
{
	QVariant positionVariant = parameterContainer_.getParameterValue("Position");

	if(positionVariant == QVariant::Invalid)
	{
		return QPoint(0,0);
	}
	else
	{
		return positionVariant.toPoint();
	}
}

QRect VisualElement::getBoundingRect()
{
	return image_.rect();
}

void VisualElement::setPosition(QPoint position)
{
	parameterContainer_.setParameterValue("Position",position);
}

void VisualElement::addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface)
{
	compositingSurface->convertImage(image_);
	compositingSurfaces_[surfaceType] = compositingSurface;
}

QSharedPointer<CompositingSurface> VisualElement::getCompositingSurface(QString surfaceType)
{
	if(compositingSurfaces_.contains(surfaceType))
	{
		return compositingSurfaces_.value(surfaceType);
	}
	else
	{
		return QSharedPointer<CompositingSurface>();
	}
}

void VisualElement::updateCompositingSurfaces()
{
	foreach(QSharedPointer<CompositingSurface> compositingSurface, compositingSurfaces_.values())
	{
		compositingSurface->convertImage(image_);
	}
}

}; //namespace Picto
