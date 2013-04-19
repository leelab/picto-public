#ifndef _AnalysisScriptContainer_H_
#define _AnalysisScriptContainer_H_

#include "../common.h"

#include "AnalysisExpLink.h"
#include "AnalysisElement.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisScriptContainer : public ScriptableContainer, public AnalysisElement
#else
class AnalysisScriptContainer : public ScriptableContainer, public AnalysisElement
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

	ANALYSIS_ELEMENT_IMPLEMENTATION

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	virtual QMap<QString,QPair<QString,QString>> getScripts();
};


}; //namespace Picto

#endif
