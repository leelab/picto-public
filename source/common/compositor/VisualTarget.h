#ifndef _VISUALTARGET_H_
#define _VISUALTARGET_H_

#include <QColor>
#include "../common.h"

#include "CompositingSurface.h"

#include <QSharedPointer>
#include <QWidget>

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API VisualTarget :  public QWidget
#else
class VisualTarget : public QWidget
#endif
{
public:
	VisualTarget(bool _bWindowed = false, int _width = 800, int _height = 600);
	virtual ~VisualTarget(){};

	virtual QSharedPointer<CompositingSurface> generateCompositingSurface() = 0;
	virtual QString getTypeName() = 0;
	virtual void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface) = 0;
	virtual void present() = 0;
	virtual QRect getDimensions();

	//! \todo Add a function that returns time to VSynch

protected:
	bool bWindowed_;
	int width_;
	int height_;
};


}; //namespace Picto

#endif
