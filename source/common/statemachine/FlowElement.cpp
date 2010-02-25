#include "FlowElement.h"

namespace Picto {

bool conditionLessThan(const FlowElement::Condition &c1, const FlowElement::Condition &c2) 
{ 
	return c1.order < c2.order; 
}

FlowElement::FlowElement()
{
	type_ = "FlowElement";
}

QString FlowElement::run()
{
	//The Condition list is sorted by the order values, so all we have to do is run 
	//through it evaluating conditions.
	foreach(Condition c, conditions_)
	{
		if(c.isCompound)
		{
			if(c.compExpr->evaluate())
			{
				return c.name;
			}
		}
		else
		{
			if(c.predExpr->evaluate())
			{
				return c.name;
			}
		}
	}

	//if we made it this far, nothing evaluated true, so return an empty string
	return "";

}

bool FlowElement::addCondition(QSharedPointer<PredicateExpression> predExpr, int order, QString name)
{
	Condition newCond;
	newCond.isCompound = false;
	newCond.predExpr = predExpr;
	newCond.order = order;
	newCond.name = name;

	//Make sure that some variant of this condition doesn't already exist
	foreach(Condition c, conditions_)
	{
		if(c.name == newCond.name)
			return false;
		if(c.order == newCond.order)
			return false;
	}

	conditions_.push_back(newCond);

	//sorting everytime we do an insert is pretty inefficient.  However, the inserts
	//will all occur at the beginning, and there aren't likely to be more than a few
	//Conditions.  Also, it's better to sort here, than when the FlowElement is run
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	return true;
}

bool FlowElement::addCondition(QSharedPointer<CompoundExpression> compExpr, int order, QString name)
{
	Condition newCond;
	newCond.isCompound = true;
	newCond.compExpr = compExpr;
	newCond.order = order;
	newCond.name = name;

	//Make sure that some variant of this condition doesn't already exist
	foreach(Condition c, conditions_)
	{
		if(c.name == newCond.name)
			return false;
		if(c.order == newCond.order)
			return false;
	}

	conditions_.push_back(newCond);

	//sorting everytime we do an insert is pretty inefficient.  However, the inserts
	//will all occur at the beginning, and there aren't likely to be more than a few
	//Conditions.  Also, it's better to sort here, than when the FlowElement is run
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	return true;
}

}; //namespace Picto
