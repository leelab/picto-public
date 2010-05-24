#include "VisualTarget.h"

namespace Picto {

VisualTarget::VisualTarget(bool _bWindowed, int _width, int _height) :
	bWindowed_(_bWindowed),
	width_(_width),
	height_(_height)
{
}

VisualTarget::~VisualTarget()
{
	printf("Visual target destructor");
}

QRect VisualTarget::getDimensions()
{
	return(QRect(0,0,width_,height_));
}

}; //namespace Picto
