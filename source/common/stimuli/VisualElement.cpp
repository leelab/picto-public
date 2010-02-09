#include "VisualElement.h"

namespace Picto {

VisualElement::VisualElement() :
	shouldUpdateCompositingSurfaces_(true),
	propertyContainer_("Visual Element")
{
}

VisualElement::~VisualElement()
{
}

QPoint VisualElement::getPosition()
{
	QVariant positionVariant = propertyContainer_.getPropertyValue("Position");

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
	propertyContainer_.setPropertyValue("Position",position);
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

//If updateAnimation isn't implemented in the derived class, we can simply return.
void VisualElement::updateAnimation(int frame, QTime elapsedTime)
{
	Q_UNUSED(frame);
	Q_UNUSED(elapsedTime);
	if(shouldUpdateCompositingSurfaces_)
	{
		updateCompositingSurfaces();
	}
	return;
}

}; //namespace Picto
