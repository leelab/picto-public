#include "FlowElement.h"
#include "Result.h"

namespace Picto {

bool conditionLessThan(const FlowElement::Condition &c1, const FlowElement::Condition &c2) 
{ 
	return c1.order < c2.order; 
}

FlowElement::FlowElement()
{
	propertyContainer_.setPropertyValue("Type","FlowElement");
	
	//At some point, we may want to make the default result name user modifiable...
	defaultResult_ = "default";
}
/*FlowElement::FlowElement(QSharedPointer<ParameterContainer> parameters)
: StateMachineElement(parameters)
{
	propertyContainer_.setPropertyValue("Type","FlowElement");
	
	//At some point, we may want to make the default result name user modifiable...
	defaultResult_ = "default";
	QSharedPointer<Result> r(new Result());
	r->setName(defaultResult_);
	addResult(r);

}*/

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

	//if we made it this far, nothing evaluated true, so return the default result
	return defaultResult_;

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
	newCond.predExpr->setParameterContainer(&parameterContainer_);

	conditions_.push_back(newCond);

	//sorting everytime we do an insert is pretty inefficient.  However, the inserts
	//will all occur at the beginning, and there aren't likely to be more than a few
	//Conditions.  Also, it's better to sort here, than when the FlowElement is run
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	//add the condition name to our list of results (even though we aren't
	//really using the result list in this case...)
	QSharedPointer<Result> r(new Result());
	r->setName(name);
	addResult(r);

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
	
	newCond.compExpr->setParameterContainer(&parameterContainer_);

	conditions_.push_back(newCond);

	//sorting everytime we do an insert is pretty inefficient.  However, the inserts
	//will all occur at the beginning, and there aren't likely to be more than a few
	//Conditions.  Also, it's better to sort here, than when the FlowElement is run
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	//add the condition name to our list of results (even though we aren't
	//really using the result list in this case...)
	QSharedPointer<Result> r;
	r->setName(name);
	addResult(r);

	return true;
}


/*	\brief Converts this FlowElement into an XML fragment
 *
 *	The FlowElement will look something like this when converted to XML:
 *		<StateMachineElement type = "FlowElement">
 *			<Name>Trial Type Branch</Name>
 *			<Conditions>
 *				<Condition>
 *					<Name>Condition 1</Name>
 *					<Order>1</Order>
 *					<CompoundExpression>
 *						...
 *					<CompoundExpression>
 *				</Condition>
 *				<Condition>
 *					<Name>Condition 2</Name>
 *					<Order>2</Order>
 *					<Expression>
 *						...
 *					</Expression>
 *				</Condition>
 *			</Conditions>
 *			<Results>
 *				<Result>
 *					<Name>Condition 1</Name>
 *				</Result>
 *				<Result>
 *					<Name>Condition 2</Name>
 *				</Result>
 *			<Results>
 *		</StateMachineElement>
 */
bool FlowElement::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachineElement");
	xmlStreamWriter->writeAttribute("type","FlowElement");

	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	xmlStreamWriter->writeStartElement("Conditions");

	foreach(Condition cond, conditions_)
	{
		xmlStreamWriter->writeStartElement("Condition");
		xmlStreamWriter->writeTextElement("Name", cond.name);
		xmlStreamWriter->writeTextElement("Order",QString("%1").arg(cond.order));
		if(cond.isCompound)
			cond.compExpr->serializeAsXml(xmlStreamWriter);
		else
			cond.predExpr->serializeAsXml(xmlStreamWriter);
		xmlStreamWriter->writeEndElement(); //Condition
	}
	xmlStreamWriter->writeEndElement(); //Conditions
	serializeResults(xmlStreamWriter);
	xmlStreamWriter->writeEndElement(); //StateMachineElement
	
	return true;
}

/*	\brief Converts an XML fragment into a FlowElement */
bool FlowElement::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
	{
		addError("FlowElement","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type())
	{
		addError("FlowElement","Incorrect type of StateMachineElement, expected FlowElement",xmlStreamReader);
		return false;
	}

	//clear out the existing results and conditions
	conditions_.clear();
	results_.clear();

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Name")
		{
			propertyContainer_.setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
		}
		else if(name == "Conditions")
		{
			//do nothing
		}
		else if(name == "Results")
		{
			if(!deserializeResults(xmlStreamReader))
			{
				addError("FlowElement", "Failed to deserialize <Results>", xmlStreamReader);
				return false;
			}
		}
		else if(name == "Condition")
		{
			QString nameStr;
			int order;
			QSharedPointer<PredicateExpression> predExpr;
			QSharedPointer<CompoundExpression> compExpr;

			bool initName = false;
			bool initOrder = false;
			bool initCompExpr = false;
			bool initPredExpr = false;

			xmlStreamReader->readNext();
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Condition") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				name = xmlStreamReader->name().toString();
				if(name == "Name")
				{
					nameStr = xmlStreamReader->readElementText();
					initName = true;
				}
				else if(name == "Order")
				{
					order = xmlStreamReader->readElementText().toInt();
					initOrder = true;
				}
				else if(name == "CompoundExpression")
				{
					compExpr = QSharedPointer<CompoundExpression>(new CompoundExpression);
					if(!compExpr->deserializeFromXml(xmlStreamReader))
					{
						addError("FlowElement", "Failed to deserialize <CompoundExpression>", xmlStreamReader);
						return false;
					}
					compExpr->setParameterContainer(&parameterContainer_);
					initCompExpr = true;
				}
				else if(name == "Expression")
				{
					predExpr = QSharedPointer<PredicateExpression>(new PredicateExpression);
					if(!predExpr->deserializeFromXml(xmlStreamReader))
					{
						addError("FlowElement", "Failed to deserialize <Expression>", xmlStreamReader);
						return false;
					}
					predExpr->setParameterContainer(&parameterContainer_);
					initPredExpr = true;
				}
				else
				{
					addError("FlowElement", "Unexpected tag within <Condition>",xmlStreamReader);
				}
				xmlStreamReader->readNext();
			}

			//if we're at the end, return false
			if(xmlStreamReader->atEnd())
			{
				addError("FlowElement", "Unexpected end of document encountered while reading <Condition>", xmlStreamReader);
				return false;
			}

			//if stuff didn't get initialized, return false
			if(!initName)
			{
				addError("FlowElement", "Did not find <Name> within <Condition>", xmlStreamReader);
				return false;
			}
			if(!initOrder)
			{
				addError("FlowElement", "Did not find <Order> within <Condition>", xmlStreamReader);
				return false;
			}
			if(!(initCompExpr ^ initPredExpr))
			{
				addError("FlowElement", "Did not find an expression within <Condition>", xmlStreamReader);
				return false;
			}
			if(initCompExpr)
				addCondition(compExpr,order,nameStr);
			else
				addCondition(predExpr,order,nameStr);
		}
		else
		{
			addError("FlowElement", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}
	
	if(xmlStreamReader->atEnd())
	{
		addError("FlowElement", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	//sort the condition list
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	return true;
}

}; //namespace Picto
