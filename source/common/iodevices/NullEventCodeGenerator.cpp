#include "NullEventCodeGenerator.h"

#include <QtDebug>

namespace Picto
{

NullEventCodeGenerator::NullEventCodeGenerator()
{
}

NullEventCodeGenerator::~NullEventCodeGenerator()
{
}

int NullEventCodeGenerator::getBitsPerEvent()
{
	return 7;
}

void NullEventCodeGenerator::sendEvent(unsigned int eventCode)
{
	qDebug()<<"Event code generated: "<<eventCode;
}

} //namespace Picto