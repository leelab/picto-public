#ifndef _LINEGRAPHIC_H_
#define _LINEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../parameter/ParameterContainer.h"
#include "../parameter/Parameter.h"

#include <QVector>
#include <QPoint>

namespace Picto {

class PICTOLIB_CLASS LineGraphic : public VisualElement
{
	Q_OBJECT

public:
	LineGraphic(QPoint position, QVector<QPoint>, QColor color);

	void draw();

private slots:
	void slotParameterValueChanged(QString parameterName, QVariant parameterValue);
};


}; //namespace Picto

#endif
