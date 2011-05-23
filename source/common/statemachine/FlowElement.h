#ifndef _FLOWELEMENT_H_
#define _FLOWELEMENT_H_

#include <QSharedPointer>

#include "../common.h"
#include "StateMachineElement.h"
#include "PredicateExpression.h"
#include "CompoundExpression.h"
#include "../engine/PictoEngine.h"

namespace Picto {
/*!	\brief A state machine element for branching based on Parameter values
 *
 *	Flow Elements are used for branching in a state machine.  Each flow element
 *	contains multiple "conditions"  Each condition contains a predicate based 
 *	expression (either a simple PredicateExpression, or a mor complex CompoundExpression)
 *	When the run is called on a flow element, it works throw the conditions (in a user
 *	defined order) until one of them evaluates true.  The element then returns the
 *	name of the condition that evaluated true as its result.  If none of the conditions
 *	evaluate true, then a default value is returned.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API FlowElement : public StateMachineElement
#else
class FlowElement : public StateMachineElement
#endif
{
public:
	FlowElement();
	//FlowElement(QSharedPointer<ParameterContainer> parameters);
	static QSharedPointer<Serializable> Create(){return QSharedPointer<Serializable>(new FlowElement());};
	
	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);

	bool addCondition(QSharedPointer<PredicateExpression> predExpr);
	bool addCondition(QSharedPointer<CompoundExpression> compExpr);

	////DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);


	typedef struct
	{
		bool isCompound;
		QSharedPointer<PredicateExpression> predExpr;
		QSharedPointer<CompoundExpression> compExpr;
		int order;
		QString name;
	} Condition;

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<Condition> conditions_;



};


}; //namespace Picto

#endif
