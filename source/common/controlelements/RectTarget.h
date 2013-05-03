#ifndef _RECTTARGET_H_
#define _RECTTARGET_H_
#include <QRect>

#include "../common.h"
#include "controltarget.h"

namespace Picto {

/*!	\brief A RectTarget is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API RectTarget : public ControlTarget
#else
class RectTarget : public ControlTarget
#endif
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
public:
	RectTarget();
	static QSharedPointer<Asset> Create();
	QRect getBounds();
	virtual QString friendlyTypeName(){return "Rectangle Target";};
	void draw();
	virtual bool contains(int x, int y);

	int getWidth() { return getBounds().width(); };
	void setWidth(int width);
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
