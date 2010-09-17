#ifndef _ANALYSISMODULE_H_
#define _ANALYSISMODULE_H_

#include "../common.h"

namespace Picto {

/*!	\brief The base class for all analysis modules.
 *
 *	An analysis module will perform analysis on the data being collected in
 *	an expirment.  Analysis can be online (occuring while the experiment is running
 *	or offline (occurring after the experiment is complete).  It is expected
 *	that most analysis modules will use a Picto Engine with null rendering targets
 *	to loop through the data.
 *
 *	The first analysis module that should be written is one that outputs the experiment
 *	to a couple of text files that can be read by matlab.
 */
class AnalysisModule
{
public:
	AnalysisModule();
};


}; //namespace Picto

#endif
