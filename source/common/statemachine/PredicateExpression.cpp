#include "PredicateExpression.h"

#include <QFont>
#include <QPainter>

namespace Picto {

PredicateExpression::PredicateExpression()
{
	LHSinitialized_ = false;
	RHSinitialized_ = false;
	LHSvalue_ = 0;
	RHSvalue_ = 0;
	LHSname_ = "";
	RHSname_ = "";

	predicate_ = 0;
}

PredicateExpression::PredicateExpression(Predicate *p)
{
	LHSinitialized_ = false;
	RHSinitialized_ = false;
	LHSvalue_ = 0;
	RHSvalue_ = 0;
	LHSname_ = "";
	RHSname_ = "";

	predicate_ = p;
}

PredicateExpression::PredicateExpression(Predicate *p, double LHSvalue, double RHSvalue, 
	QString LHSname, QString RHSname)
{
	LHSinitialized_ = true;
	RHSinitialized_ = true;
	LHSvalue_ = LHSvalue;
	RHSvalue_ = RHSvalue;
	LHSname_ = LHSname;
	RHSname_ = RHSname;

	predicate_ = p;
}

PredicateExpression::~PredicateExpression()
{
}


bool PredicateExpression::isValid()
{
	if(!predicate_)
		return false;
	return (LHSinitialized_ && RHSinitialized_);
}

bool PredicateExpression::evaluate()
{
	return predicate_->evaluate(LHSvalue_, RHSvalue_);
}



QString PredicateExpression::toString(bool useLHSName, bool useRHSName)
{
	if(!predicate_ || !LHSinitialized_ || ! RHSinitialized_)
		return "";

	QString LHSstring, RHSstring;
	if(useLHSName && !LHSname_.isEmpty())
		LHSstring = LHSname_;
	else
		LHSstring = QString("%1").arg(LHSvalue_);

	if(useRHSName && !RHSname_.isEmpty())
		RHSstring = RHSname_;
	else
		RHSstring = QString("%1").arg(RHSvalue_);
	
	return LHSstring+" "+predicate_->toString()+" "+RHSstring;
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
	QImage predImage = predicate_->toQImage();

	//----------------------
	//create left side image
	//----------------------
	QString LHSstring;
	if(useLHSName && !LHSname_.isEmpty())
		LHSstring = LHSname_;
	else
		LHSstring = QString("%1").arg(LHSvalue_);

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
	if(useRHSName && !RHSname_.isEmpty())
		RHSstring = RHSname_;
	else
		RHSstring = QString("%1").arg(RHSvalue_);

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

/*! \breife Converts a predicate expression into XML
 *
 *	This takes the current predicate expression and turns it into an
 *	XML fragment.  Bear in mind that each side of the expression can
 *	have a name and/or value.
 *
 *	SAMPLE (for the expression "trial number < 100")
 *		<PredicateExpression>
 *			<LHS name="trial number">12</LHS>
 *			<Comparison>Greater than</Comparison>
 *			<RHS name="">100</RHS>
 *
 */
bool PredicateExpression::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	return false;
}

bool PredicateExpression::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return false;
}


}; //namespace Picto
