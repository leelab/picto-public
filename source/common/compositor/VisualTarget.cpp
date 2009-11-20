#include "VisualTarget.h"

namespace Picto {

VisualTarget::VisualTarget(bool _bWindowed, int _width, int _height) :
	bWindowed_(_bWindowed),
	width_(_width),
	height_(_height)
{
}

QRect VisualTarget::getDimensions()
{
	return(QRect(0,0,width_,height_));
}

}; //namespace Picto
