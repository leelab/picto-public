#include "simplemarksource.h"
#include "..\..\common\storage\AlignmentDataUnit.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a SimpleMarkSource object that will generate AlignmentDataUnit objects once 
 *	for every input number of seconds.
 *	\note We use a default randomization here of 1ms for random delays in the message from the
 *	behavioral computer to the Neural system.  Also, the secPerSample input is sort of meaningless 
 *	here, so we might want to get rid of it.
 */
SimpleMarkSource::SimpleMarkSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample,.001)
{
	currEventCode = 0;
}

/*! \brief Creates a single AlignmentDataUnit at the input time with event code increasing by one each time and wrapping
 *	to 1 when it reaches 128.
 */
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