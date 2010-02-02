#ifndef _RANDOMLYFILLEDGRIDGRAPHIC_H_
#define _RANDOMLYFILLEDGRIDGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS RandomlyFilledGridGraphic : public VisualElement
{
	Q_OBJECT

public:
	RandomlyFilledGridGraphic(QPoint position, QRect dimensions, QColor color1, QColor color2, 
								int numHorizSquares, int numVertSquares,int numColor1, 
								bool animated, int updateFrameRate);

	void draw();
	void updateAnimation(int frame, QTime elapsedTime);

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
private:
	QList<unsigned char> colorList;
};


}; //namespace Picto

#endif
