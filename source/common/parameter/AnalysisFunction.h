#ifndef _AnalysisFunction_H_
#define _AnalysisFunction_H_

#include "../common.h"

#include "../statemachine/ScriptFunction.h"
#include "AssociateElement.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisFunction : public ScriptFunction, public AssociateElement
#else
class AnalysisFunction : public ScriptFunction, public AssociateElement
#endif
{
	Q_OBJECT

public:
	AnalysisFunction();
	virtual ~AnalysisFunction(){};

	static QSharedPointer<Asset> Create();
	virtual bool isPartOfSearch(SearchRequest searchRequest);
	virtual QString getUITemplate(){return "ScriptFunction";};
	virtual QString friendlyTypeName(){return "Script Function";};
	virtual QString getUIGroup(){return "Logic Elements";};
	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "AnalysisFunction";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
