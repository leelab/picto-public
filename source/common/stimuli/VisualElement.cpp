#include <QScriptValue>

#include "VisualElement.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new VisualElement object with the input position and color.
 *	\details Adds the following Properties, all of which are editable by the operator at runtime if the VisualElement is UIEnabled:
 *	- Position: Defines the position of the VisualElement in the Scene where 0,0 is the top left corner and 800,600 is the bottom right corner.
 *		It would have been a good idea to make Picto resolution independent, but this didn't happen.  Hopefully someone will be able to do this
 *		someday.
 *	- Color: Defines the color of the VisualElement.
 *	- Layer: Defines the layer of the VisualElement where elements with higher layers are drawn on top of elements with lower layers.
 *	- Visible: Defines whether anyone can see this VisualElement.
 *	- OperatorView: Defines whether the Operator can see the VisualElement when its Visible Property is true.
 *	- SubjectView: Defines whether the Subject can see the VisualElement when its Visible Property is true.
 */
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

/*! \brief Returns the position of the Visual Element, or 0,0 if the Position is invalid.*/
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

/*! \brief Returns a rectangle that bounds the image that the VisualElement represents.*/
QRect VisualElement::getBoundingRect()
{
	return image_.rect();
}

/*! \brief Sets the position of the VisualElement.*/
void VisualElement::setPosition(QPoint position)
{
	propertyContainer_->setPropertyValue("Position",position);
}
/*! \brief Gets the color of the VisualElement.*/
QColor VisualElement::getColor()
{
	return propertyContainer_->getPropertyValue("Color").value<QColor>();
}
/*! \brief Sets the color of the VisualElement.*/
void VisualElement::setColor(QColor color)
{
	propertyContainer_->setPropertyValue("Color",color);
}

/*! \brief Sets the color of the VisualElement.*/
void VisualElement::setColor(QVariant color)
{
	setColor(color.value<QColor>());
}

/*! \brief Returns whether the object is currently visible to the input user.
 *	\details The input is true for subject, false for operator
 */
bool VisualElement::getVisibleByUser(bool subject)
{
	if(subject)
		return getVisible() && getSubjectView();
	return getVisible() && getOperatorView();

}

/*! \brief Sets this Visual Element scalable/non-scalable.  Visual Elements are scalable by default.
 *	\details The only reason a visual element would be non-scalable is if it is a kind of UI only element that should not
 *	scale down with the rest of the scene.  This is used for example with the red cursor that is visible
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

/*! \brief Adds a CompositingSurface on which the Image represented by this VisualElement will be drawn.
 *	\details surfaceType is a string identifying the input compositingSurface type.
*/
void VisualElement::addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface)
{
	compositingSurface->convertImage(image_);
	compositingSurfaces_[surfaceType] = compositingSurface;
	if(!scalable())
		compositingSurface->dontAllowScaling();
}

/*! \brief Gets the CompositingSurface that was added to this VisualElement (addCompositingSurface()) with the input surfaceType name.
 *	\details If not such CompositingSurface is found, an empty pointer is returned.
*/
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

/*! \brief Causes the image represented by this VisualElement to be redrawn on all of its CompositingSurface objects.
 *	\details When the VisualElement contains some kind of animation, changes to the image need to be transfered to 
 *	the CompositingSurfaces.  This function can be used for that purpose.
 */
void VisualElement::updateCompositingSurfaces()
{
	foreach(QSharedPointer<CompositingSurface> compositingSurface, compositingSurfaces_.values())
	{
		compositingSurface->convertImage(image_);
	}
}

/*! \brief Causes a VisualElement with animation to update its underlying image object according to the input timing data.
 *	\details When a change to the image occurs, shouldUpdateCompositingSurfaces_ can be set true, after which VisualElement::updateAnimation()
 *	will make sure that all CompositingSurfaces are updated with the changed image (updateCompositingSurfaces()).
 */
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

/*! \brief Called when any of the VisualElement's Property values change.  If the changed Property is the Name or Position Property,
 *	draw() is called to redraw the underlying image.
 *	\details It would be a good idea to look into this to see why its actually necessary.  draw() just updates the image itself, not
 *	its position, so it shouldn't matter if the Position changed.  I'm not sure why anything has to change if the Property Name is changed.
 */
void VisualElement::slotPropertyValueChanged(Property* prop,QVariant)
{
	if(prop->getName() != "Position" && prop->getName() != "Name")
	{
		draw();
	}
}

}; //namespace Picto

