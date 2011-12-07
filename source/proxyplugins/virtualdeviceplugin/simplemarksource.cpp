#include "simplemarksource.h"
#include "..\..\common\storage\AlignmentDataUnit.h"
#include "../../common/memleakdetect.h"

SimpleMarkSource::SimpleMarkSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{
	currEventCode = 0;
}
QSharedPointer<Picto::DataUnit> SimpleMarkSource::buildEvent(double time)
{
	QSharedPointer<Picto::AlignmentDataUnit> newEvent(new Picto::AlignmentDataUnit());
	currEventCode++;
	if(currEventCode == 128)
		currEventCode = 1;
	newEvent->setAlignCode(currEventCode);
	newEvent->setTimestamp(time);
	return newEvent;
}