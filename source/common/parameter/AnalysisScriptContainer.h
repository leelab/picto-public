#ifndef _AnalysisScriptHolder_H_
#define _AnalysisScriptHolder_H_

#include "../common.h"

#include "AssociateHostLink.h"
#include "AssociateElement.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisScriptHolder : public ScriptableContainer, public AssociateElement
#else
class AnalysisScriptHolder : public ScriptableContainer, public AssociateElement
#endif
{
	Q_OBJECT

public:
	AnalysisScriptHolder();
	virtual ~AnalysisScriptHolder(){};

	static QSharedPointer<Asset> Create();
	enum ScriptType {ENTRY,FRAME,EXIT};
	void runScript(ScriptType type);
	//Returns true if this object contains a property of the input type, whether or not it is empty.
	bool hasScriptPropertyType(ScriptType type);
	virtual bool isPartOfSearch(SearchRequest searchRequest);

	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "AnalysisScriptHolder";};
	virtual QString getReturnValueError(QString scriptName,const QScriptValue& returnValue);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	virtual QMap<QString,QString> getScripts();
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
private:
	QString getScriptNamePrefix();
private slots:
	void linkedToAsset(QSharedPointer<Asset> asset);

};


}; //namespace Picto

#endif
