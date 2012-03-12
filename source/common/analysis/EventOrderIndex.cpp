#include "EventOrderIndex.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

EventOrderIndex::EventOrderIndex()
{
	time_ = -1;
	dataId_ = 0;
}
EventOrderIndex::EventOrderIndex(qulonglong dataId)
{
	time_ = -1;
	dataId_ = dataId;
}
EventOrderIndex::EventOrderIndex(double time)
{
	time_ = time;
	dataId_ = 0;
}
EventOrderIndex::EventOrderIndex(qulonglong dataId, double time)
{
	time_ = time;
	dataId_ = dataId;
}
EventOrderIndex::~EventOrderIndex()
{
}