#ifndef _BOXGRAPHIC_H_
#define _BOXGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../parameter/ParameterContainer.h"
#include "../parameter/Parameter.h"

namespace Picto {

class PICTOLIB_CLASS BoxGraphic : public VisualElement
{
	Q_OBJECT

public:
	BoxGraphic(QPoint position, QRect dimensions, QColor color);

	void draw();

private slots:
	void slotParameterValueChanged(QString parameterName, QVariant parameterValue);
};


}; //namespace Picto

#endif
