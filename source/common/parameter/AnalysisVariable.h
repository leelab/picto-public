#ifndef _ANALYSISVARIABLE_H_
#define _ANALYSISVARIABLE_H_

#include "../common.h"

#include "AssociateElement.h"
#include "../parameter/Parameter.h"

namespace Picto {

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
