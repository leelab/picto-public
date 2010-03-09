#include "CompoundExpression.h"

#include <QPainter>
#include <QImage>

namespace Picto {

CompoundExpression::CompoundExpression()
{
	LHSisPred_ = true;
	RHSisPred_ = true;

	operator_ = CompoundExpressionOperator::and;
	invertLHS_ = false;
	invertRHS_ = false;

	LHSInitialized_ = false;
	RHSInitialized_ = false;
	operatorInitialized_ = false;

}


void CompoundExpression::setLHSCompoundExp(QSharedPointer<CompoundExpression> exp, bool invert)
{
	LHSisPred_ = false;
	LHSComExp_ = exp;
	invertLHS_ = invert;
	LHSInitialized_ = true;
}

void CompoundExpression::setRHSCompoundExp(QSharedPointer<CompoundExpression> exp, bool invert)
{
	RHSisPred_ = false;
	RHSComExp_ = exp;
	invertRHS_ = invert;
	RHSInitialized_ = true;
}

void CompoundExpression::setLHSPredicateExp(QSharedPointer<PredicateExpression> exp, bool invert)
{
	LHSisPred_ = true;
	LHSPredExp_ = exp;
	invertLHS_ = invert;
	LHSInitialized_ = true;
}

void CompoundExpression::setRHSPredicateExp(QSharedPointer<PredicateExpression> exp, bool invert)
{
	RHSisPred_ = true;
	RHSPredExp_ = exp;
	invertRHS_ = invert;
	RHSInitialized_ = true;
}

void CompoundExpression::setParameterContainer(ParameterContainer *params)
{
	if(LHSisPred_)
		LHSPredExp_->setParameterContainer(params);
	else
		LHSComExp_->setParameterContainer(params);

	if(RHSisPred_)
		RHSPredExp_->setParameterContainer(params);
	else
		RHSComExp_->setParameterContainer(params);
}

bool CompoundExpression::evaluate()
{
	bool LHSresult;
	bool RHSresult;

	//evaluate the left side
	if(LHSisPred_)
		LHSresult = LHSPredExp_->evaluate();
	else
		LHSresult = LHSComExp_->evaluate();

	//evaluate the right side
	if(RHSisPred_)
		RHSresult = RHSPredExp_->evaluate();
	else
		RHSresult = RHSComExp_->evaluate();

	//invert the results if necessary
	if(invertLHS_)
		LHSresult = !LHSresult;
	if(invertRHS_)
		RHSresult = !RHSresult;

	//return the final result
	if(operator_ == CompoundExpressionOperator::and)
		return LHSresult & RHSresult;
	else if(operator_ == CompoundExpressionOperator::or)
		return LHSresult | RHSresult;
	else
		return false;
}

/*!	\brief Converts the compound expression into a string
 *
 *	Converting a compound expression into a string is generally a
 *	simple recursive call.  However, the use of names vs values is
 *	a real challenge.  Even in a simple compound expression, there
 *	are four possible values that could be names or values.  In a 
 *	larger expression, there are a limitless number of possibilities.
 *	So, to simplify matters, the parameters will apply to all LHS
 *	values and all RHS values in the compound expression.  This is less
 *	limiting than it sounds, since if the name is an empty string, the 
 *	program will print the value instead.
 */

QString CompoundExpression::toString(bool useLHSNames, bool useRHSNames)
{
	QString leftStr = "";
	if(invertLHS_)
		leftStr = "!";
	if(LHSisPred_)
		leftStr += "( " + LHSPredExp_->toString(useLHSNames,useRHSNames) + " )";
	else
		leftStr += "( " + LHSComExp_->toString(useLHSNames,useRHSNames) + " )";

	QString rightStr = "";
	if(invertRHS_)
		rightStr = "!";
	if(RHSisPred_)
		rightStr += "( " + RHSPredExp_->toString(useLHSNames,useRHSNames) + " )";
	else
		rightStr += "( " + RHSComExp_->toString(useLHSNames,useRHSNames) + " )";
	
	QString operatorStr;

	if(operator_ == CompoundExpressionOperator::and)
		operatorStr = " & ";
	else if(operator_ == CompoundExpressionOperator::or)
		operatorStr = " | ";
	else
		operatorStr = " ";

	return leftStr + operatorStr + rightStr;
}
/*!	\brief Converts the compound expression into a QImage
 *
 *	As with the toString function, this function allows us to determine whether
 *	to display the name or value for all left and right sides (but not individual
 *	ones)
 */
QImage CompoundExpression::toQImage(bool useLHSNames, bool useRHSNames)
{
	QPainter p;
	QFont font("Helvetica",72,QFont::Normal,false);

	//Generate the "(" and ")" images, and the & or | image
	//(depending on the operator)
	QImage openParenImg(50,100,QImage::Format_ARGB32);
	QImage closeParenImg(50,100,QImage::Format_ARGB32);
	QImage operatorImg(90,100,QImage::Format_ARGB32);
	
	openParenImg.fill(0x00FFFFFF);
	closeParenImg.fill(0x00FFFFFF);
	operatorImg.fill(0x00FFFFFF);

	QRect bounding(0,0,50,100);
	QRect opBounding(0,0,90,100);

	p.begin(&openParenImg);
	p.setFont(font);
	p.setBrush(QColor(0,0,0,255));
	p.setPen(QColor(0,0,0,255));
	p.drawText(bounding,Qt::AlignLeft | Qt::AlignVCenter, "(");
	p.end();

	p.begin(&closeParenImg);
	p.setFont(font);
	p.setBrush(QColor(0,0,0,255));
	p.setPen(QColor(0,0,0,255));
	p.drawText(bounding,Qt::AlignRight | Qt::AlignVCenter, ")");
	p.end();

	p.begin(&operatorImg);
	p.setFont(font);
	p.setBrush(QColor(0,0,0,255));
	p.setPen(QColor(0,0,0,255));
	if(operator_ == CompoundExpressionOperator::and)
		p.drawText(opBounding,Qt::AlignCenter | Qt::AlignVCenter, "&");
	else
		p.drawText(opBounding,Qt::AlignCenter | Qt::AlignVCenter, "|");
	p.end();

	//if needed generate the inversion operator (!)
	QImage invertImg(50,100,QImage::Format_ARGB32);
	invertImg.fill(0x00FFFFFF);

	if(invertLHS_ | invertRHS_)
	{
		p.begin(&invertImg);
		p.setFont(font);
		p.setBrush(QColor(0,0,0,255));
		p.setPen(QColor(0,0,0,255));
		p.drawText(bounding,Qt::AlignRight | Qt::AlignVCenter, "!");
		p.end();
	}

	//generate the LHS and RHS images
	QImage LHSImg, RHSImg;

	if(LHSisPred_)
		LHSImg = LHSPredExp_->toQImage(useLHSNames,useRHSNames);
	else
		LHSImg = LHSComExp_->toQImage(useLHSNames,useRHSNames);

	if(RHSisPred_)
		RHSImg = RHSPredExp_->toQImage(useLHSNames,useRHSNames);
	else
		RHSImg = RHSComExp_->toQImage(useLHSNames,useRHSNames);

	//figure out the width of the final iamge, and generate it
	//(There are gauranteed to be 4 parenthesis and one operator)
	int width = LHSImg.width() + RHSImg.width() 
				+ 2*openParenImg.width() + 2*closeParenImg.width()
				+ operatorImg.width();
	if(invertLHS_)
		width += invertImg.width();
	if(invertRHS_)
		width += invertImg.width();

	QImage compoundImg(width, 100, QImage::Format_ARGB32);
	compoundImg.fill(0x00FFFFFF);


	//put everything together (Boy I wish we could blit...)
	for(int y=0; y<100; y++)
	{
		unsigned int *destPixel = (unsigned int*)compoundImg.scanLine(y);
		unsigned int *srcPixel;

		//copy the inversion image (if needed)
		if(invertLHS_)
		{
			srcPixel = (unsigned int*)invertImg.scanLine(y);
			for(int x=0; x<invertImg.width(); x++)
			{
				*destPixel = *srcPixel;
				destPixel++;
				srcPixel++;
			}

		}

		//copy an open parenthesis
		srcPixel = (unsigned int*)openParenImg.scanLine(y);
		for(int x=0; x<openParenImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}


		//copy the left side image
		srcPixel = (unsigned int*)LHSImg.scanLine(y);
		for(int x=0; x<LHSImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}

		//copy a closed parenthesis
		srcPixel = (unsigned int*)closeParenImg.scanLine(y);
		for(int x=0; x<closeParenImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}

		//copy the operator
		srcPixel = (unsigned int*)operatorImg.scanLine(y);
		for(int x=0; x<operatorImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}

		//copy the inversion image (if needed)
		if(invertRHS_)
		{
			srcPixel = (unsigned int*)invertImg.scanLine(y);
			for(int x=0; x<invertImg.width(); x++)
			{
				*destPixel = *srcPixel;
				destPixel++;
				srcPixel++;
			}

		}

		//copy an open parenthesis
		srcPixel = (unsigned int*)openParenImg.scanLine(y);
		for(int x=0; x<openParenImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}


		//copy the right side image
		srcPixel = (unsigned int*)RHSImg.scanLine(y);
		for(int x=0; x<RHSImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}

		//copy a closed parenthesis
		srcPixel = (unsigned int*)closeParenImg.scanLine(y);
		for(int x=0; x<closeParenImg.width(); x++)
		{
			*destPixel = *srcPixel;
			destPixel++;
			srcPixel++;
		}

	}

	return compoundImg;
}

/*!	\brief	Converts a CompoundExpression into XML
 *
 *	The XML output of a compound expression will look like this:
 *	
 *	<CompoundExpression boolean="and" invertLeft="true" invertRight = "false>
 *		<Expression>
 *			...
 *		</Expression>
 *		<CompoundExpression boolean="or" invertLeft="false" invertRight = "false>
 *			<Expression>
 *				...
 *			</Expression>
 *			<Expression>
 *				...
 *			</Expression>
 *		</CompoundExpression>
 *	</CompoundExpression>
 */
bool CompoundExpression::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("CompoundExpression");

	if(operator_ == CompoundExpressionOperator::and)
		xmlStreamWriter->writeAttribute("boolean","and");
	else if(operator_ == CompoundExpressionOperator::or)
		xmlStreamWriter->writeAttribute("boolean","or");

	if(invertLHS_)
		xmlStreamWriter->writeAttribute("invertLeft","true");
	else
		xmlStreamWriter->writeAttribute("invertLeft","false");

	if(invertRHS_)
		xmlStreamWriter->writeAttribute("invertRight","true");
	else
		xmlStreamWriter->writeAttribute("invertRight","false");

	if(LHSisPred_)
		LHSPredExp_->serializeAsXml(xmlStreamWriter);
	else
		LHSComExp_->serializeAsXml(xmlStreamWriter);

	if(RHSisPred_)
		RHSPredExp_->serializeAsXml(xmlStreamWriter);
	else
		RHSComExp_->serializeAsXml(xmlStreamWriter);


	xmlStreamWriter->writeEndElement(); //CompoundExpression
	return true;
}

/*!	\brief	Converts an XML fragment into a CompoundExpression
 *
 *	The XML fragment of a compound expression will look like this:
 *	
 *	<CompoundExpression boolean="and" invertLeft="true" invertRight = "false>
 *		<Expression>
 *			...
 *		</Expression>
 *		<CompoundExpression boolean="or" invertLeft="false" invertRight = "false>
 *			<Expression>
 *				...
 *			</Expression>
 *			<Expression>
 *				...
 *			</Expression>
 *		</CompoundExpression>
 *	</CompoundExpression>
 */
bool CompoundExpression::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "CompoundExpression")
	{
		addError("CompoundExpression","Incorrect tag, expected <CompoundExpression>",xmlStreamReader);
		return false;
	}

	//read in the attributes
	QString operatorStr = xmlStreamReader->attributes().value("boolean").toString();
	if(operatorStr == "and")
		operator_ = CompoundExpressionOperator::and;
	else if(operatorStr == "or")
		operator_ = CompoundExpressionOperator::or;
	else
	{
		addError("CompoundExpression","Unexpected boolean operator.", xmlStreamReader);
		return false;
	}

	QString invertLeftStr = xmlStreamReader->attributes().value("invertLeft").toString();
	if(invertLeftStr == "true")
		invertLHS_ = true;
	else if(invertLeftStr == "false")
		invertLHS_ = false;
	else
	{
		addError("CompoundExpression","Unexpected or missing value for invertLeft", xmlStreamReader);
		return false;
	}

	QString invertRightStr = xmlStreamReader->attributes().value("invertRight").toString();
	if(invertRightStr == "true")
		invertRHS_ = true;
	else if(invertRightStr == "false")
		invertRHS_ = false;
	else
	{
		addError("CompoundExpression","Unexpected or missing value for invertRight", xmlStreamReader);
		return false;
	}

	LHSInitialized_ = false;
	RHSInitialized_ = false;
	
	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "CompoundExpression") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//do nothing unless we're looking at a start element
			xmlStreamReader->readNext();
			continue;
		}

		//use the name to set up the sides and predicate.
		//Note that we assume the order is LHS, RHS
		QString name = xmlStreamReader->name().toString();

		if(name == "Expression")
		{
			QSharedPointer<Picto::PredicateExpression> newExpr(new Picto::PredicateExpression);
			if(!newExpr->deserializeFromXml(xmlStreamReader))
				return false;
			if(!LHSInitialized_)
			{
				LHSisPred_ = true;
				LHSPredExp_ = newExpr;
				LHSInitialized_ = true;
			}
			else if(LHSInitialized_ && !RHSInitialized_)
			{
				RHSisPred_ = true;
				RHSPredExp_ = newExpr;
				RHSInitialized_ = true;
			}
			else 
			{
				addError("CompoundExpression", "Too many Expressions", xmlStreamReader);
				return false;
			}
		}
		else if(name == "CompoundExpression")
		{
			QSharedPointer<Picto::CompoundExpression> newExpr(new Picto::CompoundExpression);
			if(!newExpr->deserializeFromXml(xmlStreamReader))
				return false;
			if(!LHSInitialized_)
			{
				LHSisPred_ = false;
				LHSComExp_ = newExpr;
				LHSInitialized_ = true;
			}
			else if(LHSInitialized_ && !RHSInitialized_)
			{
				RHSisPred_ = false;
				RHSComExp_ = newExpr;
				RHSInitialized_ = true;
			}
			else 
			{
				addError("CompoundExpression", "Too many CompoundExpressions", xmlStreamReader);
				return false;
			}
		}
		else
		{
			addError("CompoundExpression", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();

	}

	if(xmlStreamReader->atEnd())
	{
		addError("CompoundExpression", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	if(!LHSInitialized_ & RHSInitialized_)
	{
		addError("CompoundExpression", "Missing one or both sides of the expression", xmlStreamReader);
		return false;
	}
	return true;
}

}; //namespace Picto
