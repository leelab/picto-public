#ifndef _CURSORGRAPHIC_H_
#define _CURSORGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"
#include "../engine/SignalChannel.h"

namespace Picto {

/*! \brief A cursor graphic used to displays the current location of an input (typically the position signal).
 *
 *	The cursor graphic displays the current x,y location of an input as taken
 *	from a signal channel and is not meant to be used as part of an Experiment design.  It was designed and 
 *	tested for use in an engine running in slave mode.
 *
 *	This graphic is set as non-scalable (VisualElement::setScalable()) since we want the cross to remain
 *	the same size regardless of the current viewer zoom state.
 *	\note It might be a good idea to add a "tail" to the position signal at some point since this has proven
 *	somewhat useful in Orion.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS CursorGraphic : public VisualElement
{
	Q_OBJECT

public:
	CursorGraphic(QSharedPointer<SignalChannel> channel, QColor color=QColor());

	static const QString type;

	void draw();
	void updateAnimation(int frame, QTime elapsedTime);
	QPoint getPositionOffset();

	virtual QString friendlyTypeName(){return "Cursor Graphic";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	

private:
	QSharedPointer<SignalChannel> positionChannel_;
	int size_;
};


}; //namespace Picto

#endif
