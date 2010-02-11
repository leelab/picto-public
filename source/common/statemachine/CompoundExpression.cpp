#include "CompoundExpression.h"

#include <QPainter>
#include <QImage>

namespace Picto {

CompoundExpression::CompoundExpression(PredicateExpression *LHS, PredicateExpression *RHS, 
			CompoundExpressionOperator::CompoundExpressionOperator op,
			bool invertLHS, bool invertRHS)
{
	LHS_.isPred = true;
	LHS_.predExp = LHS;
	RHS_.isPred = true;
	RHS_.predExp = RHS;

	operator_ = op;
	invertLHS_ = invertLHS;
	invertRHS_ = invertRHS;
}

CompoundExpression::CompoundExpression(CompoundExpression *LHS, PredicateExpression *RHS, 
			CompoundExpressionOperator::CompoundExpressionOperator op,
			bool invertLHS, bool invertRHS)
{
	LHS_.isPred = false;
	LHS_.comExp = LHS;
	RHS_.isPred = true;
	RHS_.predExp = RHS;

	operator_ = op;
	invertLHS_ = invertLHS;
	invertRHS_ = invertRHS;
}

CompoundExpression::CompoundExpression(PredicateExpression *LHS, CompoundExpression *RHS, 
			CompoundExpressionOperator::CompoundExpressionOperator op,
			bool invertLHS, bool invertRHS)
{
	LHS_.isPred = true;
	LHS_.predExp = LHS;
	RHS_.isPred = false;
	RHS_.comExp = RHS;

	operator_ = op;
	invertLHS_ = invertLHS;
	invertRHS_ = invertRHS;
}

CompoundExpression::CompoundExpression(CompoundExpression *LHS, CompoundExpression *RHS, 
			CompoundExpressionOperator::CompoundExpressionOperator op,
			bool invertLHS, bool invertRHS)
{
	LHS_.isPred = false;
	LHS_.comExp = LHS;
	RHS_.isPred = false;
	RHS_.comExp = RHS;

	operator_ = op;
	invertLHS_ = invertLHS;
	invertRHS_ = invertRHS;
}

void CompoundExpression::setLHSCompoundExp(CompoundExpression *exp, bool invert)
{
	LHS_.isPred = false;
	LHS_.comExp = exp;
	invertLHS_ = invert;
}

void CompoundExpression::setRHSCompoundExp(CompoundExpression *exp, bool invert)
{
	RHS_.isPred = false;
	RHS_.comExp = exp;
	invertRHS_ = invert;
}

void CompoundExpression::setLHSPredicateExp(PredicateExpression *exp, bool invert)
{
	LHS_.isPred = true;
	LHS_.predExp = exp;
	invertLHS_ = invert;
}

void CompoundExpression::setRHSPredicateExp(PredicateExpression *exp, bool invert)
{
	RHS_.isPred = true;
	RHS_.predExp = exp;
	invertRHS_ = invert;
}

bool CompoundExpression::evaluate()
{
	bool LHSresult;
	bool RHSresult;

	//evaluate the left side
	if(LHS_.isPred)
		LHSresult = LHS_.predExp->evaluate();
	else
		LHSresult = LHS_.comExp->evaluate();

	//evaluate the right side
	if(RHS_.isPred)
		RHSresult = RHS_.predExp->evaluate();
	else
		RHSresult = RHS_.comExp->evaluate();

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
	if(LHS_.isPred)
		leftStr += "( " + LHS_.predExp->toString(useLHSNames,useRHSNames) + " )";
	else
		leftStr += "( " + LHS_.comExp->toString(useLHSNames,useRHSNames) + " )";

	QString rightStr = "";
	if(invertRHS_)
		rightStr = "!";
	if(RHS_.isPred)
		rightStr += "( " + RHS_.predExp->toString(useLHSNames,useRHSNames) + " )";
	else
		rightStr += "( " + RHS_.comExp->toString(useLHSNames,useRHSNames) + " )";
	
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

	if(LHS_.isPred)
		LHSImg = LHS_.predExp->toQImage(useLHSNames,useRHSNames);
	else
		LHSImg = LHS_.comExp->toQImage(useLHSNames,useRHSNames);

	if(RHS_.isPred)
		RHSImg = RHS_.predExp->toQImage(useLHSNames,useRHSNames);
	else
		RHSImg = RHS_.comExp->toQImage(useLHSNames,useRHSNames);

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





}; //namespace Picto
