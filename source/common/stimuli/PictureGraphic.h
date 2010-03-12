#ifndef _PICTUREGRAPHIC_H_
#define _PICTUREGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS PictureGraphic : public VisualElement
{
	Q_OBJECT

public:
	PictureGraphic(QPoint position=QPoint(), QString imageFile="");

	void draw();
	static VisualElement* NewVisualElement();

	static const QString type;

public slots:
	QString getImageFile() { return propertyContainer_.getPropertyValue("ImageFile").toString(); };
	void setImageFile(QString filename) { propertyContainer_.setPropertyValue("ImageFile",filename);};

private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
