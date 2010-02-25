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


/*	\brief Converts this FlowElement into an XML fragment
 *
 *	The FlowElement will look something like this when converted to XML:
 *		<FlowElement>
 *			<ID>{07B5A6FE-EE99-4f26-8F65-9537E5A5E2E3}</ID>
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
 *					<PredicateExpression>
 *						...
 *					</PredicateExpression>
 *				</Condition>
 *			</Conditions>
 *		</FlowElement>
 */
bool FlowElement::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FlowElement");

	xmlStreamWriter->writeTextElement("ID",getId().toString());
	xmlStreamWriter->writeTextElement("Name",name_);

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
	xmlStreamWriter->writeEndElement(); //FlowElement
	
	return true;
}

/*	\brief Converts an XML fragment into a FlowElement
 *
 *	The FlowElement will look something like this when converted to XML:
 *		<FlowElement>
 *			<ID>{07B5A6FE-EE99-4f26-8F65-9537E5A5E2E3}</ID>
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
 *					<PredicateExpression>
 *						...
 *					</PredicateExpression>
 *				</Condition>
 *			</Conditions>
 *		</FlowElement>
 */
bool FlowElement::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "FlowElement")
		return false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "FlowElement") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "ID")
		{
			QString idStr = xmlStreamReader->readElementText();
			setId(QUuid(idStr));
			registerObject();
		}
		else if(name == "Name")
		{
			name_ = xmlStreamReader->readElementText();
		}
		else if(name == "Conditions")
		{
			//do nothing
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
					compExpr->deserializeFromXml(xmlStreamReader);
					initCompExpr = true;
				}
				else if(name == "PredicateExpression")
				{
					predExpr = QSharedPointer<PredicateExpression>(new PredicateExpression);
					predExpr->deserializeFromXml(xmlStreamReader);
					initPredExpr = true;
				}
				else
				{
					return false;
				}
			}

			//if we're at the end, return false
			if(xmlStreamReader->atEnd())
				return false;

			//if stuff didn't get initialized, return false
			if(!(initName && initOrder && (initCompExpr ^ initPredExpr)))
				return false;

			Condition cond;
			cond.isCompound = initCompExpr;
			cond.name = nameStr;
			cond.order = order;
			if(initCompExpr)
				cond.compExpr = compExpr;
			else
				cond.predExpr = predExpr;

			conditions_.push_back(cond);
		}
		else
		{
			return false;
		}
		xmlStreamReader->readNext();
	}
	
	if(xmlStreamReader->atEnd())
		return false;

	//sort the condition list
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	return true;
}

}; //namespace Picto
