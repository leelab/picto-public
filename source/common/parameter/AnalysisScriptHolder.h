#ifndef _AnalysisScriptHolder_H_
#define _AnalysisScriptHolder_H_

#include "../common.h"

#include "AssociateHostLink.h"
#include "AssociateElement.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*! \brief A container for scripts that are used in an Analysis.
 *	\details This class doesn't have an analogy in the experimental design.  In the experimental design, StateMachineElement objects contain
 *	Entry,Exit and Frame (for State objects) script Properties that are directly used by the StateMachineElement.  With Analyses, we want
 *	to be able to dynamically add and remove Analysis Entry, Exit and Frame scripts depending on whether an Analyisis is enabled or not.
 *	For this reason, we created this container class that holds the Entry, Exit and Frame Scripts and can be added or removed from a 
 *	StateMachineElement just like any other AssociateElement.  The StateMachineElement can then simply check if it has a child AssociateElement
 *	that is an AnalysisScriptHolder with an active Associate ID in order to decide if it needs to run the Analysis scripts or not.
 *
 *	Since it does contain scripts and needs to be able to run them in the script engine, this class inherits ScriptableContainer.
 *	\sa ScriptableContainer, StateMachineElement
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	/*! \brief An enum describing the types of scripts available in this class.*/
	enum ScriptType {
		ENTRY,	//!< An entry script that runs just before the design entry script.
		FRAME,	//!< A frame script that runs just after each frame is presented.
		EXIT	//!< An exit script that runs just after the design exit script.
	};
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
