/*!	\brief A state machine element for branching based on Parameter values
 *
 *	Flow Elements are used for branching in a state machine.  Each flow element
 *	contains multiple "conditions"  Each condition contains a predicate based 
 *	expression (either a simple PredicateExpression, or a mor complex CompoundExpression)
 *	When the run is called on a flow element, it works throw the conditions (in a user
 *	defined order) until one of them evaluates true.  The element then returns the
 *	name of the condition that evaluated true as its result.  If none of the conditions
 *	evaluate true, then a defulat value is returned.
 */
#ifndef _FLOWELEMENT_H_
#define _FLOWELEMENT_H_

#include <QSharedPointer>

#include "../common.h"
#include "StateMachineElement.h"
#include "PredicateExpression.h"
#include "CompoundExpression.h"
#include "../engine/PictoEngine.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API FlowElement : public StateMachineElement
#else
class FlowElement : public StateMachineElement
#endif
{
public:
	FlowElement();
	//FlowElement(QSharedPointer<ParameterContainer> parameters);
	
	QString run(QSharedPointer<Engine::PictoEngine> engine);

	bool addCondition(QSharedPointer<PredicateExpression> predExpr, int order, QString name);
	bool addCondition(QSharedPointer<CompoundExpression> compExpr, int order, QString name);

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	typedef struct
	{
		bool isCompound;
		QSharedPointer<PredicateExpression> predExpr;
		QSharedPointer<CompoundExpression> compExpr;
		int order;
		QString name;
	} Condition;

private:
	QList<Condition> conditions_;



};


}; //namespace Picto

#endif
