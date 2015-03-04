#ifndef _RECTTARGET_H_
#define _RECTTARGET_H_
#include <QRect>

#include "../common.h"
#include "controltarget.h"

namespace Picto {

/*!	\brief A ControlTarget with a rectangular geometry.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API RectTarget : public ControlTarget
#else
class RectTarget : public ControlTarget
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the target's current width.*/
	Q_PROPERTY(int width READ getWidth WRITE setWidth);
	/*! \brief Sets/Gets the target's current width.*/
	Q_PROPERTY(int height READ getHeight WRITE setHeight);
public:
	RectTarget();
	static QSharedPointer<Asset> Create();
	QRect getBounds();
	virtual QString getUITemplate(){return "RectangleTarget";};
	virtual QString friendlyTypeName(){return "Rectangle Target";};
	void draw();
	virtual bool contains(int x, int y);

	/*! \brief Gets the target's current width.*/
	int getWidth() { return getBounds().width(); };
	void setWidth(int width);
	/*! \brief Gets the target's current height.*/
	int getHeight() { return getBounds().height(); };
	void setHeight(int height);
	QPoint getPositionOffset();
protected:
	void setBounds(QRect bounds);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QPoint posOffset_;
	bool active_;
};


}; //namespace Picto

#endif
