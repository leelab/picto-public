#ifndef _AnalysisScriptContainer_H_
#define _AnalysisScriptContainer_H_

#include "../common.h"

#include "AssociateExpLink.h"
#include "AssociateElement.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisScriptContainer : public ScriptableContainer, public AssociateElement
#else
class AnalysisScriptContainer : public ScriptableContainer, public AssociateElement
#endif
{
	Q_OBJECT

public:
	AnalysisScriptContainer();
	virtual ~AnalysisScriptContainer(){};

	static QSharedPointer<Asset> Create();
	enum ScriptType {ENTRY,FRAME,EXIT};
	void runScript(ScriptType type);
	//Returns true if this object contains a property of the input type, whether or not it is empty.
	bool hasScriptPropertyType(ScriptType type);
	virtual bool isPartOfSearch(SearchRequest searchRequest);

	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual QString getReturnValueError(QString scriptName,const QScriptValue& returnValue);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	virtual QMap<QString,QPair<QString,QString>> getScripts();
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
private:
	QString getScriptNamePrefix();
};


}; //namespace Picto

#endif
