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
	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
