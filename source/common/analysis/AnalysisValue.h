#ifndef _ANALYSIS_VALUE_H_
#define _ANALYSIS_VALUE_H_

#include "EventOrderIndex.h"

namespace Picto {
//AnalysisValue is the parent class for all AnalysisDataStore values that 
//are read in from the session.
struct AnalysisValue
{
	AnalysisValue(){};
	AnalysisValue(EventOrderIndex id){index=id;};
	EventOrderIndex index;
};

}; //namespace Picto
#endif