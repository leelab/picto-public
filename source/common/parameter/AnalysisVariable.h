#ifndef _ANALYSISVARIABLE_H_
#define _ANALYSISVARIABLE_H_

#include "../common.h"

#include "AssociateElement.h"
#include "../parameter/Parameter.h"

namespace Picto {

/*! \brief The base class of all variables that can be used in an Analysis.
 *	\details Works exactly like a Parameter with the exception that it is part of an Analysis.
 *	\sa Parameter
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVariable : public Parameter, public AssociateElement
#else
class AnalysisVariable : public Parameter, public AssociateElement
#endif
{
	Q_OBJECT

public:
	AnalysisVariable();
	virtual ~AnalysisVariable(){};

	virtual void enteredScope();
	virtual bool isPartOfSearch(SearchRequest searchRequest);
	virtual QString getUITemplate(){return "Variable";};
	virtual QString getUIGroup(){return "Variables";};
	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "AnalysisVariable";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
