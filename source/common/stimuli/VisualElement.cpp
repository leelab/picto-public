#include "VisualElement.h"

#include <QScriptValue>

namespace Picto {

VisualElement::VisualElement(QPoint position, QColor color) :
	shouldUpdateCompositingSurfaces_(true)
{
	AddDefinableProperty(QVariant::Point,"Position",position);
	AddDefinableProperty(QVariant::Color,"Color",color);
	AddDefinableProperty(QVariant::Int,"Layer",0);
	AddDefinableProperty(QVariant::Bool,"Visible",true);
	AddDefinableProperty(QVariant::Bool,"OperatorView",true);
	AddDefinableProperty(QVariant::Bool,"SubjectView",true);
}

VisualElement::~VisualElement()
{
}

QPoint VisualElement::getPosition()
{
	QVariant positionVariant = propertyContainer_->getPropertyValue("Position");
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
	propertyContainer_->setPropertyValue("Position",position);
}

QColor VisualElement::getColor()
{
	return propertyContainer_->getPropertyValue("Color").value<QColor>();
}

void VisualElement::setColor(QColor color)
{
	propertyContainer_->setPropertyValue("Color",color);
}

/*! \brief Returns whether the object is visible to the input user.
 *	The input is true for subject, false for operator
 */
bool VisualElement::getVisibleByUser(bool subject)
{
	if(subject)
		return getVisible() && getSubjectView();
	return getVisible() && getOperatorView();

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

void VisualElement::postSerialize()
{
	Scriptable::postSerialize();
	draw();
	connect(propertyContainer_.data(),
	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	    this,
		SLOT(slotPropertyValueChanged(QString, int, QVariant))
		);

	setPropertyRuntimeEditable("Position");
	setPropertyRuntimeEditable("Color");
	setPropertyRuntimeEditable("Layer");
	setPropertyRuntimeEditable("Visible");
	setPropertyRuntimeEditable("OperatorView");
	setPropertyRuntimeEditable("SubjectView");
}

bool VisualElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	return true;
}

void VisualElement::slotPropertyValueChanged(QString propertyName, int,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

}; //namespace Picto

