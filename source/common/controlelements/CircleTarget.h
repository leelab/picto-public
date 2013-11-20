#ifndef _CIRCLETARGET_H_
#define _CIRCLETARGET_H_
#include <QRect>
#include "../common.h"
#include "controltarget.h"

namespace Picto {

/*!	\brief A ControlTarget with a circular geometry.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API CircleTarget : public ControlTarget
#else
class CircleTarget : public ControlTarget
#endif
{
	Q_OBJECT
	/*!	\brief Sets/Gets the circle target's current radius*/
	Q_PROPERTY(int radius READ getRadius WRITE setRadius)
public:
	CircleTarget();
	static QSharedPointer<Asset> Create();
	virtual QString friendlyTypeName(){return "Circle Target";};
	void draw();
	virtual bool contains(int x, int y);
	QPoint getPositionOffset();
	int getRadius();
	void setRadius(int radius);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QPoint posOffset_;
	bool active_;
};


}; //namespace Picto

#endif
