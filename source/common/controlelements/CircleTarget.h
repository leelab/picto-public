#ifndef _CIRCLETARGET_H_
#define _CIRCLETARGET_H_
#include <QRect>
#include "../common.h"
#include "controltarget.h"

namespace Picto {

/*!	\brief A CircleTarget is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API CircleTarget : public ControlTarget
#else
class CircleTarget : public ControlTarget
#endif
{
	Q_OBJECT
	Q_PROPERTY(int radius READ getRadius WRITE setRadius)
public:
	CircleTarget();
	static QSharedPointer<Asset> Create();
	virtual QString assetType(){return "CircleTarget";};
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
