#ifndef _PICTUREGRAPHIC_H_
#define _PICTUREGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A picture graphic.
 *
 *	A PictureGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the picture (in screen coordinates)
 *	- ImageFile: The name of the file to display
 */


class PICTOLIB_CLASS PictureGraphic : public VisualElement
{
	Q_OBJECT

public:
	PictureGraphic(QPoint position=QPoint(), QString imageFile="");

	void draw();
	static VisualElement* NewVisualElement();

	static const QString type;

public slots:
	QString getImageFile() { return propertyContainer_->getPropertyValue("ImageFile").toString(); };
	void setImageFile(QString filename) { propertyContainer_->setPropertyValue("ImageFile",filename);};

private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
