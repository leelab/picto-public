#include <QScriptValue>

#include "VisualElement.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

VisualElement::VisualElement(QPoint position, QColor color) :
	shouldUpdateCompositingSurfaces_(true),
	scalable_(true)
{
	AddDefinableProperty(QVariant::Point,"Position",position);
	AddDefinableProperty(QVariant::Color,"Color",color);
	AddDefinableProperty(QVariant::Int,"Layer",0);
	AddDefinableProperty(QVariant::Bool,"Visible",true);
	AddDefinableProperty(QVariant::Bool,"OperatorView",true);
	AddDefinableProperty(QVariant::Bool,"SubjectView",true);

	//As of version 0.0.1, we have removed the Dimensions properties from all VisualElements
	//and replaced them with Size properties.  See upgradeVersion() for details.
	AddDefinableObjectFactory("Dimensions",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
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

void VisualElement::setColor(QVariant color)
{
	setColor(color.value<QColor>());
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

/*! \Brief Sets this Visual Element scalable/non-scalable.  Visual Elements are scalable by default.
 *	The only reason a visual element would make itself non-scalable is if it is a kind of UI only element that should not
 *	scale down with the rest of the scene.  A good example of this type of functionality would be a cursor visible
 *	only by the operator.  If the operator zooms out, he doesn't want the cursor graphic
 *	to get smaller.  It should stay the same size.  For that reason, the cursor graphic is set as non-scalable.
 */
void VisualElement::setScalable(bool scalable)
{
	scalable_ = scalable;
}

void VisualElement::upgradeVersion(QString deserializedVersion)
{
	OutputElement::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{
		//As of version 0.0.1, we have removed the Dimensions properties from all VisualElements
		//and replaced them with Size properties.  We take advantage of the fact that the x,y
		//properties of the Dimensions rectangle were never used so we can just throw them out.

		//Get the old obsolete Dimensions Rect property
		QList<QSharedPointer<Asset>> oldRectProps = getGeneratedChildren("Dimensions");
		//Get the new Size property
		QList<QSharedPointer<Asset>> newSizeProps = getGeneratedChildren("Size");
		if(oldRectProps.size() && newSizeProps.size())
		{
			Q_ASSERT((oldRectProps.size() == 1) && (newSizeProps.size() == 1));
			QSharedPointer<ObsoleteAsset> oldRectProp = oldRectProps.first().staticCast<ObsoleteAsset>();
			QSharedPointer<Property> newSizeProp = newSizeProps.first().staticCast<Property>();
			//Try getting the old rect from the two methods of serializing it that were used, attributes and string
			QStringList rectStringVals;
			if(oldRectProp->getAttributeValue("x").isEmpty())
			{
				rectStringVals = oldRectProp->getValue().split(",");
			}
			else
			{
				rectStringVals << oldRectProp->getAttributeValue("x") 
					<< oldRectProp->getAttributeValue("y") 
					<< oldRectProp->getAttributeValue("width") 
					<< oldRectProp->getAttributeValue("height");
			}
			Q_ASSERT(rectStringVals.size() == 4);
			QRect oldRect(	rectStringVals[0].toInt(),
							rectStringVals[1].toInt(),
							rectStringVals[2].toInt(),
							rectStringVals[3].toInt());
			newSizeProp->setValue(QSize(oldRect.width(),oldRect.height()));
		}
	}
}

void VisualElement::addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface)
{
	compositingSurface->convertImage(image_);
	compositingSurfaces_[surfaceType] = compositingSurface;
	if(!scalable())
		compositingSurface->dontAllowScaling();
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

void VisualElement::postDeserialize()
{
	OutputElement::postDeserialize();
	draw();
	connect(propertyContainer_.data(),
	    SIGNAL(propertyValueChanged(Property*,QVariant)),
	    this,
		SLOT(slotPropertyValueChanged(Property*,QVariant))
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
	if(!OutputElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

void VisualElement::slotPropertyValueChanged(Property* prop,QVariant)
{
	if(prop->getName() != "Position" && prop->getName() != "Name")
	{
		draw();
	}
}

}; //namespace Picto

