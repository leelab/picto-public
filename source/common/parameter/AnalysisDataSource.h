#ifndef _AnalysisDataSource_H_
#define _AnalysisDataSource_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief This is a base class for all classes that can be used to access detailed data about a Picto Session.
 *	\details When analyzing a Picto session, beyond simply retrieving the current state of the StateMachine, 
 *	we need the capability to read the values of input signals and neural data.  That is the purpose of 
 *	AnalysisDataSource objects, these object are used to retrieve additional data about the experimental run
 *	beyond what is immediately readable from the state of the regular experimental variables.
 *	
 *	Currently, this class does not add any functionality to AnalysisVariable, but we wanted to create this base class
 *	since its children are conceptually similar and it may be useful to add general functionality for them in the future.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisDataSource : public AnalysisVariable
#else
class AnalysisDataSource : public AnalysisVariable
#endif
{
	Q_OBJECT

public:
	AnalysisDataSource();
	virtual ~AnalysisDataSource(){};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
