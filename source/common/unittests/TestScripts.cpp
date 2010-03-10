#include "TestScripts.h"
#include "../parameter/ParameterContainer.h"
#include "../parameter/NumericParameter.h"
#include "../parameter/BooleanParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/RangeParameter.h"

#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/PictureGraphic.h"


TestScripts::TestScripts()
{
	randGen_.seed();
}



void TestScripts::TestParameterBinding()
{
	//Setup
	//-----
	QString script;
	Picto::ParameterContainer parameters;

	//Load up the parameter container
	QSharedPointer<Picto::NumericParameter> param1(new Picto::NumericParameter);
	param1->setName("TrialNumber");
	param1->setValue(1);
	parameters.addParameter(param1);

	QSharedPointer<Picto::BooleanParameter> param2(new Picto::BooleanParameter);
	param2->setName("passed");
	param2->setValue(false);
	parameters.addParameter(param2);


	QSharedPointer<Picto::ChoiceParameter> param3(new Picto::ChoiceParameter);
	param3->setName("color");

	param3->addChoice("Red",QColor(255,0,0));
	param3->addChoice("Green",QColor(0,255,0));
	param3->addChoice("Blue",QColor(255,0,0));
	param3->setValue("Green");
	parameters.addParameter(param3);

	QSharedPointer<Picto::RangeParameter> param4(new Picto::RangeParameter);
	param4->setName("NumberOfTargets");
	param4->setMin(2);
	param4->setMax(10);
	param4->setIncrement(2);
	param4->setValue(4);
	parameters.addParameter(param4);

	//Bind everything
	parameters.bindToScriptEngine(qsEngine_);

	//Test the numeric parameter
	//--------------------------
	QSharedPointer<Picto::Parameter> testParam;
	testParam = parameters.getParameter("TrialNumber");
	QCOMPARE(testParam->getValue().toInt(),1);

	script = "TrialNumber.setValue(TrialNumber.getValue() + 5);\n";
	qsEngine_.evaluate(script);
				
	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().toInt(),6);

	//Test the boolean parameter
	//--------------------------
	testParam = parameters.getParameter("passed");
	QCOMPARE(testParam->getValue().toBool(),false);

	script = "passed.setValue(true);\n";
	qsEngine_.evaluate(script);

	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().toBool(),true);

	//Test the choice parameter
	//-------------------------
	testParam = parameters.getParameter("color");
	QCOMPARE(testParam->getValue().toString(),QString("#00ff00"));

	script = "color.setValue(\"Red\");\n";
	qsEngine_.evaluate(script);

	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().value<QColor>(),QColor(255,0,0));

	//Test the range parameter
	//------------------------
	testParam = parameters.getParameter("NumberOfTargets");
	QCOMPARE(testParam->getValue().toInt(),4);

	script = "NumberOfTargets.setValue(8);\n";
	qsEngine_.evaluate(script);
				
	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().toInt(),8);

}

/*!	\Brief Tests binding to the ArrowGraphic properties
 *
 *	The ArrowGraphic has the following bound functions that need to be tested
 *		getStartPointX()
 *		getStartPointY()
 *		setStartPoint(x,y)
 *		getEndPointX()
 *		getEndPointY()
 *		setEndPoint(x,y)
 *		getSize()
 *		setSize(size)
 */
void TestScripts::TestArrowGraphicBinding()
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);
	QSharedPointer<Picto::ArrowGraphic> arrow(new Picto::ArrowGraphic);

	TestVisualElementBinding(arrow);

	arrow->setName("TestArrow");
	arrow->bindToScriptEngine(qsEngine);

	int initialStartX = randGen_.randInt(300);
	int initialStartY = randGen_.randInt(300);
	int initialEndX = randGen_.randInt(300);
	int initialEndY = randGen_.randInt(300);
	int initialSize = randGen_.randInt(10);

	arrow->setStartPoint(QPoint(initialStartX,initialStartY));
	arrow->setEndPoint(QPoint(initialEndX,initialEndY));
	arrow->setSize(initialSize);

	QString script = "x=TestArrow.getStartPointX();"
					 "y=TestArrow.getStartPointY();"
					 "TestArrow.setStartPoint(x+5,y+5);";
	qsEngine->evaluate(script);
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(arrow->getStartPoint().x(),initialStartX+5);
	QCOMPARE(arrow->getStartPoint().y(),initialStartY+5);
	
	script = "x=TestArrow.getEndPointX();"
			 "y=TestArrow.getEndPointY();"
			 "TestArrow.setEndPoint(x+5,y+5);";
	qsEngine->evaluate(script);
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(arrow->getEndPoint().x(),initialEndX+5);
	QCOMPARE(arrow->getEndPoint().y(),initialEndY+5);

	script = "TestArrow.setSize(TestArrow.getSize()+5);";
	qsEngine->evaluate(script);
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(arrow->getSize(),initialSize+5);
}

/*!	\Brief Tests binding to the BoxGraphic properties
 *
 *	The BoxGraphic has the following bound functions that need to be tested
 *		getWidth()
 *		setWidth(w)
 *		getHeight()
 *		setHeight(h)
 */
void TestScripts::TestBoxGraphicBinding()
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);
	QSharedPointer<Picto::BoxGraphic> box(new Picto::BoxGraphic);

	TestVisualElementBinding(box);

	box->setName("TestBox");
	box->bindToScriptEngine(qsEngine);

	int initialWidth = randGen_.randInt(300);
	int initialHeight = randGen_.randInt(300);
	box->setDimensions(QRect(0,0,initialWidth,initialHeight));

	qsEngine->evaluate("TestBox.setWidth(TestBox.getWidth()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(box->getDimensions().width(),initialWidth+5);

	qsEngine->evaluate("TestBox.setHeight(TestBox.getHeight()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(box->getDimensions().height(),initialHeight+5);

}

/*!	\Brief Tests binding to the CircleGraphic properties
 *
 *	The CircleGraphic has the following bound functions that need to be tested
 *		getRadius()
 *		setRadius(r)
 */
void TestScripts::TestCircleGraphicBinding()
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);
	QSharedPointer<Picto::CircleGraphic> circle(new Picto::CircleGraphic);

	TestVisualElementBinding(circle);

	int initialRadius = randGen_.randInt(300);
	circle->setRadius(initialRadius);
	circle->setName("TestCircle");
	circle->bindToScriptEngine(qsEngine);

	qsEngine->evaluate("TestCircle.setRadius(TestCircle.getRadius()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(circle->getRadius(),initialRadius+5);
}

/*!	\Brief Tests binding to the EllipseGraphic properties
 *
 *	The EllipseGraphic has the following bound functions that need to be tested
 *		getWidth()
 *		setWidth(w)
 *		getHeight()
 *		setHeight(h)
 */
void TestScripts::TestEllipseGraphicBinding()
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);
	QSharedPointer<Picto::EllipseGraphic> ellipse(new Picto::EllipseGraphic);

	TestVisualElementBinding(ellipse);

	ellipse->setName("TestEllipse");
	ellipse->bindToScriptEngine(qsEngine);

	int initialWidth = randGen_.randInt(300);
	int initialHeight = randGen_.randInt(300);
	ellipse->setDimensions(QRect(0,0,initialWidth,initialHeight));

	qsEngine->evaluate("TestEllipse.setWidth(TestEllipse.getWidth()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(ellipse->getDimensions().width(),initialWidth+5);

	qsEngine->evaluate("TestEllipse.setHeight(TestEllipse.getHeight()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(ellipse->getDimensions().height(),initialHeight+5);

}


/*!	\Brief Tests binding to the LineGraphic properties
 *
 *	The LineGraphic has the following bound functions that need to be tested
 *		no additional functions (yet)
 */
void TestScripts::TestLineGraphicBinding()
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);
	QSharedPointer<Picto::LineGraphic> line(new Picto::LineGraphic);

	TestVisualElementBinding(line);
}

/*!	\Brief Tests binding to the PictureGraphic properties
 *
 *	The PictureGraphic has the following bound functions that need to be tested
 *		getImageFile()
 *		setImageFile(r)
 */
void TestScripts::TestPictureGraphicBinding()
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);
	QSharedPointer<Picto::PictureGraphic> picture(new Picto::PictureGraphic);

	TestVisualElementBinding(picture);

	picture->setName("TestPicture");
	picture->setImageFile("Test1.jpg");
	picture->bindToScriptEngine(qsEngine);

	qsEngine->evaluate("TestPicture.setImageFile(\"SomeOtherFile.bmp\")");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(picture->getImageFile(),QString("SomeOtherFile.bmp"));
}

/*	\brief tests the base class binding
 * 
 *	This tests the following bound functions which are part of the
 *	VisualElement base class (and hence, are found in all VisualElements)
 *		getX()
 *		getY()
 *		setPosition(x,y)
 *		getAlpha()
 *		getRed()
 *		getBlue()
 *		getGreen()
 *		setColor(r,g,b)
 *
 *	NOTE: This function completely rearranges the passed in VisualElement, so think about
 *	the order in which you call it.
 */
void TestScripts::TestVisualElementBinding(QSharedPointer<Picto::VisualElement> v)
{
	QSharedPointer<QScriptEngine> qsEngine(new QScriptEngine);

	int initialX = randGen_.randInt(500);
	int initialY = randGen_.randInt(500);
	int initialA = randGen_.randInt(200);
	int initialR = randGen_.randInt(200);
	int initialG = randGen_.randInt(200);
	int initialB = randGen_.randInt(200);

	v->setColor(QColor(initialR,initialG,initialB,initialA));
	v->setPosition(QPoint(initialX,initialY));
	v->setName("TestElement");

	v->bindToScriptEngine(qsEngine);

	qsEngine->evaluate("TestElement.setPosition(TestElement.getX()+5,TestElement.getY()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(v->getPosition().x(),initialX+5);
	QCOMPARE(v->getPosition().y(),initialY+5);

	qsEngine->evaluate("TestElement.setColor(TestElement.getRed()+5, "
											"TestElement.getGreen()+5, "
											"TestElement.getBlue()+5, "
											"TestElement.getAlpha()+5)");
	QCOMPARE(qsEngine->hasUncaughtException(),false);
	QCOMPARE(v->getColor().alpha(),initialA+5);
	QCOMPARE(v->getColor().red(),initialR+5);
	QCOMPARE(v->getColor().green(),initialG+5);
	QCOMPARE(v->getColor().blue(),initialB+5);

}
