#include "FlowElement.h"
#include "CompareToParamExpression.h"
#include "CompareToValExpression.h"

#include <QCoreApplication>

namespace Picto {

bool conditionLessThan(const FlowElement::Condition &c1, const FlowElement::Condition &c2) 
{ 
	return c1.order < c2.order; 
}

FlowElement::FlowElement()
{
	
	AddDefinableProperty("Type","FlowElement");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	addResultFactoryType("CompareParams",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompareToParamExpression::Create))));
	addResultFactoryType("CompareParamToVal",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompareToValExpression::Create))));
	addResultFactoryType("CompoundCompare",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompoundExpression::Create))));
	
	//resultFactory_->addAssetType("CompareParams",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompareToParamExpression::Create))));
	//resultFactory_->addAssetType("CompareParamToVal",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompareToValExpression::Create))));
	//resultFactory_->addAssetType("CompoundCompare",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompoundExpression::Create))));

	//AddDefinableObjectFactory("CompoundCondition",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CompoundExpression::Create))) );
	//really using the result list in this case...)
	//Add the default result
	addRequiredResult("default");
}

QString FlowElement::run(QSharedPointer<Engine::PictoEngine> engine)
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
	return "default";

}

QString FlowElement::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	while(result.isEmpty())
	{
		result = getMasterStateResult(engine);
		QCoreApplication::processEvents();
	}

	return result;
}

bool FlowElement::addCondition(QSharedPointer<PredicateExpression> predExpr)
{
	Condition newCond;
	newCond.isCompound = false;
	newCond.predExpr = predExpr;
	newCond.name = predExpr->getName();
	newCond.order = predExpr->order();

	//Make sure that some variant of this condition doesn't already exist
	foreach(Condition c, conditions_)
	{
		if(c.name == newCond.name)
			return false;
		if(c.order == newCond.order)
			return false;
	}
	addChildScriptableContainer(newCond.predExpr);

	conditions_.push_back(newCond);

	//sorting everytime we do an insert is pretty inefficient.  However, the inserts
	//will all occur at the beginning, and there aren't likely to be more than a few
	//Conditions.  Also, it's better to sort here, than when the FlowElement is run
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	////add the condition name to our list of results (even though we aren't
	////really using the result list in this case...)
	//addRequiredResult(newCond.name)));

	return true;
}

bool FlowElement::addCondition(QSharedPointer<CompoundExpression> compExpr)
{
	Condition newCond;
	newCond.isCompound = true;
	newCond.compExpr = compExpr;
	newCond.name = compExpr->getName();
	newCond.order = compExpr->order();

	//Make sure that some variant of this condition doesn't already exist
	foreach(Condition c, conditions_)
	{
		if(c.name == newCond.name)
			return false;
		if(c.order == newCond.order)
			return false;
	}
	
	addChildScriptableContainer(newCond.compExpr);

	conditions_.push_back(newCond);

	//sorting everytime we do an insert is pretty inefficient.  However, the inserts
	//will all occur at the beginning, and there aren't likely to be more than a few
	//Conditions.  Also, it's better to sort here, than when the FlowElement is run
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	////add the condition name to our list of results (even though we aren't
	////really using the result list in this case...)
	//addRequiredResult(newCond.name)));

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
//bool FlowElement::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("StateMachineElement");
//	xmlStreamWriter->writeAttribute("type","FlowElement");
//
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//
//	xmlStreamWriter->writeStartElement("Conditions");
//
//	foreach(Condition cond, conditions_)
//	{
//		xmlStreamWriter->writeStartElement("Condition");
//		xmlStreamWriter->writeTextElement("Name", cond.name);
//		xmlStreamWriter->writeTextElement("Order",QString("%1").arg(cond.order));
//		if(cond.isCompound)
//			cond.compExpr->toXml(xmlStreamWriter);
//		else
//			cond.predExpr->toXml(xmlStreamWriter);
//		xmlStreamWriter->writeEndElement(); //Condition
//	}
//	xmlStreamWriter->writeEndElement(); //Conditions
//	serializeResults(xmlStreamWriter);
//	xmlStreamWriter->writeEndElement(); //StateMachineElement
//	
//	return true;
//}
//
///*	\brief Converts an XML fragment into a FlowElement */
//bool FlowElement::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->getName() != "StateMachineElement")
//	{
//		addError("FlowElement","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != type())
//	{
//		addError("FlowElement","Incorrect type of StateMachineElement, expected FlowElement",xmlStreamReader);
//		return false;
//	}
//
//	//clear out the existing results and conditions
//	conditions_.clear();
//	results_.clear();
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->getName().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->getName().toString();
//		if(name == "Name")
//		{
//			propertyContainer_->setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
//		}
//		else if(name == "Conditions")
//		{
//			//do nothing
//		}
//		else if(name == "Results")
//		{
//			if(!deserializeResults(xmlStreamReader))
//			{
//				addError("FlowElement", "Failed to deserialize <Results>", xmlStreamReader);
//				return false;
//			}
//		}
//		else if(name == "Condition")
//		{
//			QString nameStr;
//			int order;
//			QSharedPointer<PredicateExpression> predExpr;
//			QSharedPointer<CompoundExpression> compExpr;
//
//			bool initName = false;
//			bool initOrder = false;
//			bool initCompExpr = false;
//			bool initPredExpr = false;
//
//			xmlStreamReader->readNext();
//			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->getName().toString() == "Condition") && !xmlStreamReader->atEnd())
//			{
//				if(!xmlStreamReader->isStartElement())
//				{
//					//Do nothing unless we're at a start element
//					xmlStreamReader->readNext();
//					continue;
//				}
//
//				name = xmlStreamReader->getName().toString();
//				if(name == "Name")
//				{
//					nameStr = xmlStreamReader->readElementText();
//					initName = true;
//				}
//				else if(name == "Order")
//				{
//					order = xmlStreamReader->readElementText().toInt();
//					initOrder = true;
//				}
//				else if(name == "CompoundExpression")
//				{
//					compExpr = QSharedPointer<CompoundExpression>(new CompoundExpression);
//					if(!compExpr->fromXml(xmlStreamReader))
//					{
//						addError("FlowElement", "Failed to deserialize <CompoundExpression>", xmlStreamReader);
//						return false;
//					}
//					compExpr->setParameterContainer(&parameterContainer_);
//					initCompExpr = true;
//				}
//				else if(name == "Expression")
//				{
//					predExpr = QSharedPointer<PredicateExpression>(new PredicateExpression);
//					if(!predExpr->fromXml(xmlStreamReader))
//					{
//						addError("FlowElement", "Failed to deserialize <Expression>", xmlStreamReader);
//						return false;
//					}
//					predExpr->setParameterContainer(&parameterContainer_);
//					initPredExpr = true;
//				}
//				else
//				{
//					addError("FlowElement", "Unexpected tag within <Condition>",xmlStreamReader);
//				}
//				xmlStreamReader->readNext();
//			}
//
//			//if we're at the end, return false
//			if(xmlStreamReader->atEnd())
//			{
//				addError("FlowElement", "Unexpected end of document encountered while reading <Condition>", xmlStreamReader);
//				return false;
//			}
//
//			//if stuff didn't get initialized, return false
//			if(!initName)
//			{
//				addError("FlowElement", "Did not find <Name> within <Condition>", xmlStreamReader);
//				return false;
//			}
//			if(!initOrder)
//			{
//				addError("FlowElement", "Did not find <Order> within <Condition>", xmlStreamReader);
//				return false;
//			}
//			if(!(initCompExpr ^ initPredExpr))
//			{
//				addError("FlowElement", "Did not find an expression within <Condition>", xmlStreamReader);
//				return false;
//			}
//			if(initCompExpr)
//				addCondition(compExpr,order,nameStr);
//			else
//				addCondition(predExpr,order,nameStr);
//		}
//		else
//		{
//			addError("FlowElement", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//	
//	if(xmlStreamReader->atEnd())
//	{
//		addError("FlowElement", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	//sort the condition list
//	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);
//
//	return true;
//}

void FlowElement::postSerialize()
{
	//clear out the existing conditions
	conditions_.clear();

	QList<QSharedPointer<Asset>> newExprs = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> newExpr,newExprs)
	{
		if(newExpr.dynamicCast<PredicateExpression>())
		{
			addCondition(newExpr.staticCast<PredicateExpression>());
		}
		else if (newExpr.dynamicCast<CompoundExpression>())
		{
			addCondition(newExpr.staticCast<CompoundExpression>());
		}
		else
			addRequiredResult(newExpr.staticCast<Result>());
	}
	//Sort the condition list
	qSort(conditions_.begin(), conditions_.end(), &conditionLessThan);

	StateMachineElement::postSerialize();
}

bool FlowElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Validate Results
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	QList<QSharedPointer<Asset>> results = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		if(!result.dynamicCast<PredicateExpression>() && 
			!result.dynamicCast<CompoundExpression>() &&
			result->getName() != "default")
		{
			QString errMsg = QString("FlowElement: %1 has a result that is not an expression or default: %2").arg(getName()).arg(result->getName());
			addError("FlowElement", errMsg, xmlStreamReader);
		}
	}
	return true;
}

}; //namespace Picto
