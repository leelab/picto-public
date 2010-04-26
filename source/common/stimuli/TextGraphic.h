/*!	\brief Generates text stimuli.
 *
 *	Obviously this wouldn't be used in experiments with non-human
 *	subjects, but it can be very useful for debugging, providing 
 *	info on the operator screen, or running experiments with human
 *	subjects.
 */

#ifndef _TEXTGRAPHIC_H_
#define _TEXTGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS TextGraphic : public VisualElement
{
	Q_OBJECT

public:
	TextGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor(), QString text=QString());

	void draw();
	static VisualElement* NewVisualElement();

	QRect getDimensions();
	void setDimensions(QRect dimensions);

	static const QString type;

public slots:
	int getWidth() { return getDimensions().width(); };
	void setWidth(int width);
	int getHeight() { return getDimensions().height(); };
	void setHeight(int height);
	QString getText();
	void setText(QString text);

private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
