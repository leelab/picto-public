#include "Predicate.h"

namespace Picto {

Predicate::Predicate()
{
	QImage* temp = new QImage(100,100,QImage::Format_ARGB32);
	icon_ = temp->copy();
	icon_.fill(0x00FFFFFF);
	delete temp;

	predicateString_ = "";
}

Predicate::~Predicate()
{
}

}; //namespace Picto
