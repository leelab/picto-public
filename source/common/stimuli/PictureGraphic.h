#ifndef _PICTUREGRAPHIC_H_
#define _PICTUREGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../parameter/ParameterContainer.h"
#include "../parameter/Parameter.h"

namespace Picto {

class PICTOLIB_CLASS PictureGraphic : public VisualElement
{
	Q_OBJECT

public:
	PictureGraphic(QPoint position, QString imageFile);

	void draw();

private slots:
	void slotParameterValueChanged(QString parameterName, QVariant parameterValue);
};


}; //namespace Picto

#endif
