#ifndef _CIRCLEGRAPHIC_H_
#define _CIRCLEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../parameter/ParameterContainer.h"
#include "../parameter/Parameter.h"

namespace Picto {

class PICTOLIB_CLASS CircleGraphic : public VisualElement
{
	Q_OBJECT

public:
	CircleGraphic(QPoint position, int radius, QColor color);

	void draw();

private slots:
	void slotParameterValueChanged(QString parameterName, QVariant parameterValue);
};


}; //namespace Picto

#endif
