#include "simplemarksource.h"
#include "..\..\common\storage\AlignmentDataStore.h"

SimpleMarkSource::SimpleMarkSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{
	currEventCode = 0;
}
QSharedPointer<Picto::DataStore> SimpleMarkSource::buildEvent(double time)
{
	QSharedPointer<Picto::AlignmentDataStore> newEvent(new Picto::AlignmentDataStore());
	currEventCode++;
	if(currEventCode == 128)
		currEventCode = 1;
	newEvent->setAlignCode(currEventCode);
	newEvent->setTimestamp(time);
	return newEvent;
}