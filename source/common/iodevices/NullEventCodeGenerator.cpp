#include <QtDebug>
#include "NullEventCodeGenerator.h"
#include "../memleakdetect.h"

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

/*! \brief Implements EventCodeGenerator::sendEvent() to simply print
 *	out the input eventCode to the debug console.
 */
double NullEventCodeGenerator::sendEvent(unsigned int eventCode)
{
	qDebug()<<"Event code generated: "<<eventCode;
	return 0.0;
}

} //namespace Picto