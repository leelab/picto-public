#include "VisualTarget.h"
#include "../memleakdetect.h"

namespace Picto {

VisualTarget::VisualTarget(bool _bWindowed, int _width, int _height) :
	bWindowed_(_bWindowed),
	width_(_width),
	height_(_height),
	lastFrameTime_(-1.0),
	zoom_(1.0)
{
	int i=0;
	i++;
}

VisualTarget::~VisualTarget()
{
	printf("Visual target destructor");
}

void VisualTarget::setZoom(float zoom)
{
	Q_ASSERT(zoom > 0);
	zoom_ = zoom;
}

QRect VisualTarget::getDimensions()
{
	return(QRect(0,0,width_,height_));
}

QPoint VisualTarget::viewportPointToTargetPoint(QPoint viewportPoint)
{
	return viewportPoint;
}

QPoint VisualTarget::targetPointToViewportPoint(QPoint targetPoint)
{
	return targetPoint;
}

void VisualTarget::setFirstPhosphorTime()
{
	lastFrameTime_ = stamper_.stampSec();
}

}; //namespace Picto
