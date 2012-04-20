#include "EventOrderIndex.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

EventOrderIndex::EventOrderIndex()
{
	time_ = INT_MIN;
	dataId_ = 0;
	idSource_ = EventOrderIndex::BEHAVIORAL;
}
EventOrderIndex::EventOrderIndex(double time)
{
	time_ = time;
	dataId_ = 0;
}
EventOrderIndex::EventOrderIndex(double time, qulonglong dataId, IDSource idSource)
{
	time_ = time;
	dataId_ = dataId;
	idSource_ = idSource;
}
EventOrderIndex::~EventOrderIndex()
{
}