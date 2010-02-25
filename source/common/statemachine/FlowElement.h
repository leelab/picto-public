#ifndef _FLOWELEMENT_H_
#define _FLOWELEMENT_H_

#include <QSharedPointer>

#include "../common.h"
#include "StateMachineElement.h"
#include "PredicateExpression.h"
#include "CompoundExpression.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API FlowElement : public StateMachineElement
#else
class FlowElement : public StateMachineElement
#endif
{
public:
	FlowElement();
	
	QString run();

	bool addCondition(QSharedPointer<PredicateExpression> predExpr, int order, QString name);
	bool addCondition(QSharedPointer<CompoundExpression> compExpr, int order, QString name);
	typedef struct
	{
		bool isCompound;
		QSharedPointer<PredicateExpression> predExpr;
		QSharedPointer<CompoundExpression> compExpr;
		int order;
		QString name;
	} Condition;

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<Condition> conditions_;



};


}; //namespace Picto

#endif
