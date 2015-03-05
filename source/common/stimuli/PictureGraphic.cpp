#include <QPainter>

#include "PictureGraphic.h"
#include "../memleakdetect.h"
namespace Picto {

	const QString PictureGraphic::type = "Picture Graphic";

/*! \brief Creates a PictureGraphic object at the input position based on the image at the input ImageFile path.
 *	\details Adds an ImageFile Property to hold the path to the ImageFile.
 */
PictureGraphic::PictureGraphic(QPoint position, QString imageFile)
: VisualElement(position)
{
	AddDefinableProperty("ImageFile",imageFile);
	if(imageFile != "" || (position != QPoint()))
	{
		initializePropertiesToDefaults();
		//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
		propertyContainer_->getProperty("Position")->setEdited();
		propertyContainer_->getProperty("ImageFile")->setEdited();
	}
}

/*! \brief Creates a new PictureGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> PictureGraphic::Create()
{
	return QSharedPointer<Asset>(new PictureGraphic());
}

void PictureGraphic::draw()
{
	QString imageFile = propertyContainer_->getPropertyValue("ImageFile").toString();
	image_.load(imageFile);

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

void PictureGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool PictureGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
