#include "VisualTarget.h"

namespace Picto {

VisualTarget::VisualTarget(bool _bWindowed, int _width, int _height) :
	bWindowed_(_bWindowed),
	width_(_width),
	height_(_height),
	lastFrameTime_(-1.0)
{
	int i=0;
	i++;
}

VisualTarget::~VisualTarget()
{
	printf("Visual target destructor");
}

QRect VisualTarget::getDimensions()
{
	return(QRect(0,0,width_,height_));
}

void VisualTarget::setFirstPhosphorTime()
{
	lastFrameTime_ = stamper_.stampSec();
	if(lastFrameTime_ > 10000)
	{
		int i=0;
		i++;
	}
}

}; //namespace Picto
