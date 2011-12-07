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
	Q_PROPERTY(QString imageFile READ getImageFile WRITE setImageFile)
public:
	PictureGraphic(QPoint position=QPoint(), QString imageFile="");
	static QSharedPointer<Asset> Create();
	void draw();
	static VisualElement* NewVisualElement();

	QString getImageFile() { return propertyContainer_->getPropertyValue("ImageFile").toString(); };
	void setImageFile(QString fileName) { propertyContainer_->setPropertyValue("ImageFile",fileName); };

	static const QString type;

protected:
	virtual QString defaultTagName(){return "Picture";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private slots:
	//void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
