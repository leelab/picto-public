#include "PredicateExpression.h"
#include "PredicateFactory.h"

#include <QFont>
#include <QPainter>

namespace Picto {


PredicateExpression::PredicateExpression()
{
	AddDefinableProperty("Name","");
	AddDefinableProperty(QVariant::Int,"Order",0);
	AddDefinableProperty(QVariant::String,"LeftParameter","",0,1);
	AddDefinableProperty(QVariant::String,"RightParameter","",0,1);
	AddDefinableProperty(QVariant::String,"Comparison","",0,1);
	AddDefinableProperty(QVariant::String,"Value","",0,1);

	LHSParamName_ = "";
	RHSParamName_ = "";
	useRHSVal_ = false;

	predicateName_ = "";
}

PredicateExpression::PredicateExpression(QString predicateName)
{
	PredicateFactory predFact;
	Q_ASSERT(predFact.createPredicate(predicateName));

	LHSParamName_ = "";
	RHSParamName_ = "";
	useRHSVal_ = false;

	predicateName_ = predicateName;
}

bool PredicateExpression::isValid()
{
	if(predicateName_.isEmpty())
		return false;
	if(LHSParamName_.isEmpty())
		return false;
	if(RHSParamName_.isEmpty() && !useRHSVal_)
		return false;
	return true;
}

bool PredicateExpression::evaluate()
{
	if(!isValid())
		return false;

	PredicateFactory predFact;
	QSharedPointer<Predicate> pred = predFact.createPredicate(predicateName_);

	QSharedPointer<Parameter> LHSParam = parameters_->getParameter(LHSParamName_);
	if(LHSParam.isNull())
		return false;
	if(useRHSVal_)
	{
		return pred->evaluate(*LHSParam,RHSval_);
	}
	else
	{
		QSharedPointer<Parameter> RHSParam = parameters_->getParameter(RHSParamName_);
		return pred->evaluate(*LHSParam, *RHSParam);
	}
}

QString PredicateExpression::name()
{
	return propertyContainer_->getPropertyValue("Name").toString();
}
void PredicateExpression::setName(QString name)
{
	propertyContainer_->setPropertyValue("Name",name);
}
void PredicateExpression::setOrder(int order)
{
	propertyContainer_->setPropertyValue("Order",order);
}
int PredicateExpression::order()
{
	return propertyContainer_->getPropertyValue("Order").toInt();
}

QString PredicateExpression::toString(bool useLHSName, bool useRHSName)
{
	if(!isValid())
		return "";

	PredicateFactory predFact;
	QSharedPointer<Predicate> pred = predFact.createPredicate(predicateName_);

	QSharedPointer<Parameter> LHSParam = parameters_->getParameter(LHSParamName_);

	QString LHSstring, RHSstring;
	if(useLHSName)
		LHSstring = LHSParam->name();
	else
		LHSstring = LHSParam->getValue().toString();

	if(!useRHSVal_)
	{
		QSharedPointer<Parameter> RHSParam = parameters_->getParameter(RHSParamName_);
		if(useRHSName)
			RHSstring=RHSParam->name();
		else
			RHSstring= RHSParam->getValue().toString();
	}
	else
	{
		RHSstring = RHSval_.toString();
	}

	return LHSstring+" "+pred->toString()+" "+RHSstring;
}

QImage PredicateExpression::toQImage(bool useLHSName, bool useRHSName)
{
	if(!isValid())
		return QImage();

	//Set up our font (bearing in mind that the images are 100 pixels tall)
	QFont font("Helvetica",72,QFont::Normal,false);
	QPainter p;

	//-----------------------
	//Get the predicate image
	//-----------------------
	PredicateFactory predFact;
	QSharedPointer<Predicate> pred = predFact.createPredicate(predicateName_);
	QImage predImage = pred->toQImage();

	//----------------------
	//create left side image
	//----------------------
	QString LHSstring;
	QSharedPointer<Parameter> LHSParam = parameters_->getParameter(LHSParamName_);
	if(useLHSName)
		LHSstring = LHSParam->name();
	else
		LHSstring = LHSParam->getValue().toString();

	QImage LHSimage(80*LHSstring.length(),100,QImage::Format_ARGB32);

	p.begin(&LHSimage);
	p.setFont(font);
	QRect LHSbounding = p.boundingRect(0,0,80*LHSstring.length(),100,Qt::AlignLeft | Qt::AlignVCenter,LHSstring);
	LHSbounding.setHeight(100);
	p.end();  //the end is neccesary since we're about to make a copy of the image

	LHSimage = LHSimage.scaled(LHSbounding.size());
	LHSimage.fill(0x00FFFFFF);

	p.begin(&LHSimage);
	p.setFont(font);
	p.setBrush(QColor(0,0,0,255));
	p.setPen(QColor(0,0,0,255));
	p.drawText(LHSbounding,Qt::AlignLeft | Qt::AlignVCenter, LHSstring);
	p.end();

	//-----------------------
	//create right side image
	//-----------------------
	QString RHSstring;
	QSharedPointer<Parameter> RHSParam = parameters_->getParameter(RHSParamName_);
	if(useRHSName & !useRHSVal_)
		RHSstring = RHSParam->name();
	else if(useRHSVal_)
		RHSstring = RHSval_.toString();
	else
		RHSstring = RHSParam->getValue().toString();

	QImage RHSimage(80*RHSstring.length(),100,QImage::Format_ARGB32);

	p.begin(&RHSimage);
	p.setFont(font);
	QRect RHSbounding = p.boundingRect(0,0,80*RHSstring.length(),100,Qt::AlignLeft | Qt::AlignVCenter,RHSstring);
	RHSbounding.setHeight(100);
	p.end();  //the end is neccesary since we're about to make a copy of the image

	RHSimage = RHSimage.scaled(RHSbounding.size());
	RHSimage.fill(0x00FFFFFF);

	p.begin(&RHSimage);
	p.setFont(font);
	p.setBrush(QColor(0,0,0,255));
	p.setPen(QColor(0,0,0,255));
	p.drawText(RHSbounding,Qt::AlignLeft | Qt::AlignVCenter, RHSstring);
	p.end();

	//-----------------------
	//put everything together
	//-----------------------
	int expWidth = LHSimage.width() + RHSimage.width() + predImage.width();
	int expHeight = 100;
	QImage expImage(expWidth,expHeight,QImage::Format_ARGB32);

	//we have to copy the images manually 
	//(I'm kind of surprised there isn't a blit rectangle function...)
	for(int y=0; y<100; y++)
	{
		unsigned int *destPixel = (unsigned int*)expImage.scanLine(y);

		unsigned int *LHSsrcPixel = (unsigned int*)LHSimage.scanLine(y);
		for(int x=0; x<LHSimage.width(); x++)
		{
			*destPixel = *LHSsrcPixel;
			destPixel++;
			LHSsrcPixel++;
		}

		unsigned int *predSrcPixel = (unsigned int*)predImage.scanLine(y);
		for(int x=0; x<predImage.width(); x++)
		{
			*destPixel = *predSrcPixel;
			destPixel++;
			predSrcPixel++;
		}

		unsigned int *RHSsrcPixel = (unsigned int*)RHSimage.scanLine(y);
		for(int x=0; x<RHSimage.width(); x++)
		{
			*destPixel = *RHSsrcPixel;
			destPixel++;
			RHSsrcPixel++;
		}
	}
	return expImage;
}

/*! \breif Converts a predicate expression into XML
 *
 *	This takes the current predicate expression and turns it into an
 *	XML fragment. 
 *
 *	SAMPLE 
 *		<Expression>
 *			<Parameter>Trial Number</Parameter>
 *			<Comparison>Greater than</Comparison>
 *			<Value>100</Value>
 *		</Expression>
 */
//bool PredicateExpression::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	if(!isValid())
//		return false;
//
//	PredicateFactory predFact;
//	QSharedPointer<Predicate> pred = predFact.createPredicate(predicateName_);
//	if(pred.isNull())
//		return false;
//
//
//	xmlStreamWriter->writeStartElement("Expression");
//	xmlStreamWriter->writeTextElement("Parameter",LHSParamName_);
//	xmlStreamWriter->writeTextElement("Comparison",pred->name());
//	if(useRHSVal_)
//	{
//		//Not all constant values can be converted to a string this way.  However,
//		//so far everything, works.  If you hit this assert, you'll need to do a bit
//		//of rewriting...
//		Q_ASSERT(!RHSval_.toString().isEmpty());
//		xmlStreamWriter->writeStartElement("Value");
//		xmlStreamWriter->writeAttribute("type", RHSval_.typeName());
//		xmlStreamWriter->writeCharacters(RHSval_.toString());
//		xmlStreamWriter->writeEndElement();
//	}
//	else
//		xmlStreamWriter->writeTextElement("Parameter",RHSParamName_);
//	xmlStreamWriter->writeEndElement(); //Expression
//	return true;
//}
//
///*!	\brief Converts XML into a predicate expression
// *
// *	This takes a predicate expression in unknown state and modifies it to
// *	match the XML fragment currently being read.  The function assumes
// *	that the xml stream reader is pointing to an <Expression> tag upon entry,
// *	and returns with the reader pointing to an </Expression tag>
// */
//bool PredicateExpression::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Expression")
//	{
//		addError("PredicateExpression","Incorrect tag, expected <Expression>",xmlStreamReader);
//		return false;
//	}
//
//	bool lhsInit = false;
//	bool rhsInit = false;
//	bool predInit = false;
//
//	//loop through the XML modifying properties as we go.
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Expression") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//do nothing unless we're looking at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//		QString name = xmlStreamReader->name().toString();
//
//		if(name == "Parameter")
//		{
//			//LHS parameter
//			if(!lhsInit)
//			{
//				LHSParamName_ = xmlStreamReader->readElementText();
//				lhsInit = true;
//			}
//			//RHS parameter
//			else if(lhsInit && !rhsInit)
//			{
//				RHSParamName_ = xmlStreamReader->readElementText();
//				rhsInit = true;
//				useRHSVal_ = false;
//			}
//		}
//		else if(name == "Comparison")
//		{
//			predicateName_ = xmlStreamReader->readElementText();
//			predInit = true;
//		}
//		else if(name == "Value" && lhsInit && !rhsInit)
//		{
//			QString valueTypeStr;
//			valueTypeStr = xmlStreamReader->attributes().value("type").toString();
//
//			QVariant::Type valueType = QVariant::nameToType(valueTypeStr.toLatin1());
//			if(valueType == QVariant::Invalid)
//			{
//				addError("PredicateExpression","Invalid or unrecognized type for value", xmlStreamReader);
//				return false;
//			}
//
//			//This will probably break at some point...
//			//Since the Value is written out using QVariant::toString, I can simply
//			//read it back in as a string, and then convert it to the correct type.
//			QString valueStr;
//			valueStr = xmlStreamReader->readElementText();
//			RHSval_ = valueStr;
//			RHSval_.convert(valueType);
//
//			rhsInit = true;
//			useRHSVal_ = true;
//		}
//		else
//		{
//			addError("PredicateExpression", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//	
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("PredicateExpression", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//	if(!lhsInit)
//	{
//		addError("PredicateExpression", "Left hand side of expression not defined", xmlStreamReader);
//		return false;
//	}
//	if(!rhsInit)
//	{
//		addError("PredicateExpression", "Right hand side of expression not defined", xmlStreamReader);
//		return false;
//	}
//	if(!predInit)
//	{
//		addError("PredicateExpression", "Predicate not defined", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

bool PredicateExpression::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	useRHSVal_ = true;
	if(!hasChildrenOfType("LeftParameter"))
	{
		addError("PredicateExpression","Left hand side of expression not defined",xmlStreamReader);
		return false;
	}
	if(!hasChildrenOfType("Comparison"))
	{
		addError("PredicateExpression","Predicate operator not defined",xmlStreamReader);
		return false;
	}
	if(hasChildrenOfType("RightParameter"))
	{
		useRHSVal_ = false;
	}
	if(hasChildrenOfType("Value"))
	{
		if(!useRHSVal_)
		{
			addError("PredicateExpression","Both a RightParameter and a Value were specified.",xmlStreamReader);
			return false;
		}
	}
	else if(useRHSVal_)
	{		
		addError("PredicateExpression","Right hand side of expression not defined",xmlStreamReader);
		return false;
	}

	LHSParamName_ = propertyContainer_->getPropertyValue("LeftParameter").toString();
	if(useRHSVal_)
		RHSval_ = propertyContainer_->getPropertyValue("Value").toString();
	else
		RHSParamName_ = propertyContainer_->getPropertyValue("RightParameter").toString();
		
	predicateName_ = propertyContainer_->getPropertyValue("Comparison").toString();
	return true;
}

}; //namespace Picto
