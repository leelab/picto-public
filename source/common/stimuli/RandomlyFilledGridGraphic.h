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
	RandomlyFilledGridGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), 
								QColor color1=QColor(), QColor color2=QColor(), 
								int numHorizSquares=0, int numVertSquares=0,int numColor1=0, 
								bool animated=false, int updateFrameRate=0);

	void draw();
	void updateAnimation(int frame, QTime elapsedTime);
	static VisualElement* NewVisualElement();

	static const QString name;

	private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QList<unsigned char> colorList;
};


}; //namespace Picto

#endif
