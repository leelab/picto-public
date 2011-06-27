#ifndef _CURSORGRAPHIC_H_
#define _CURSORGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"
#include "../engine/SignalChannel.h"

namespace Picto {

/*! \brief Displays the current location of an input
 *
 *	The cursor graphic displays the current location of an input as taken
 *	from a signal channel.  It was designed and tested for use in an engine
 *	running in slave mode.
 *
 *	A CursorGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the cursor (in screen coordinates)
 *	- Color: The color of the circle
 *
 *	NOTE: In its current state, the cursor graphic can be used in a state
 *	machine, but it won't be possible to automatically update its location
 * using a signal channel
 *
 *	Initially, the cursor will be a simple cross.  However, future versions 
 *	will likely inlcude multiple shapes, and should probably include the 
 *	option to have a "tail" (showing how the cursor has moved).
 */

class PICTOLIB_CLASS CursorGraphic : public VisualElement
{
	Q_OBJECT

public:
	CursorGraphic(QSharedPointer<SignalChannel> channel, QColor color=QColor());

	void draw();
	void updateAnimation(int frame, QTime elapsedTime);

	static VisualElement* NewVisualElement();

	static const QString type;

public slots:

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);

private:
	QSharedPointer<SignalChannel> positionChannel_;
};


}; //namespace Picto

#endif
