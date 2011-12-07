#include <QFile>
#include <QSharedPointer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "TestDataStore.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"

#include "../parameter/BooleanParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/RangeParameter.h"
#include "../parameter/NumericParameter.h"
//#include "../parameter/ParameterContainer.h"

//#include "../statemachine/PredicateExpression.h"
//#include "../statemachine/CompoundExpression.h"

#include "../statemachine/Scene.h"

#include "../statemachine/StateMachine.h"
#include "../statemachine/StateMachineElement.h"
#include "../statemachine/FlowElement.h"
#include "../statemachine/Reward.h"
#include "../statemachine/Result.h"
#include "../statemachine/State.h"

#include "../controlelements/ControlElementFactory.h"
#include "../controlelements/ControlElement.h"
#include "../controlelements/TestController.h"
#include "../controlelements/StopwatchController.h"
#include "../controlelements/TargetController.h"
#include "../memleakdetect.h"

Q_DECLARE_METATYPE(QSharedPointer<Picto::VisualElement>)
Q_DECLARE_METATYPE(QSharedPointer<Picto::Parameter>)

TestDataStore::TestDataStore()
{
	randGen_.seed();

	//Set up the VisualElementFactory
	visualElementFactory_.addVisualElementType(Picto::ArrowGraphic::type, &Picto::ArrowGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::BoxGraphic::type, &Picto::BoxGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::CircleGraphic::type, &Picto::CircleGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::EllipseGraphic::type, &Picto::EllipseGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::LineGraphic::type, &Picto::LineGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::PictureGraphic::type, &Picto::PictureGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::RandomlyFilledGridGraphic::type, &Picto::RandomlyFilledGridGraphic::NewVisualElement);

	//Set up the ParameterFactory
	//Since the factories map is static, we should be able to set it up once and
	//have it work even if we create a new instance
	Picto::ParameterFactory paramFactory;
	paramFactory.addParameterType("Boolean",&Picto::BooleanParameter::NewParameter);
	paramFactory.addParameterType("Choice",&Picto::ChoiceParameter::NewParameter);
	paramFactory.addParameterType("Range",&Picto::RangeParameter::NewParameter);
	paramFactory.addParameterType("Numeric",&Picto::NumericParameter::NewParameter);

	//Set up the ControlElementFactory
	Picto::ControlElementFactory controlElementFactory;
	controlElementFactory.addControlElementType(Picto::TestController::ControllerType(), &Picto::TestController::NewTestController);
	controlElementFactory.addControlElementType(Picto::StopwatchController::ControllerType(), &Picto::StopwatchController::NewStopwatchController);
	controlElementFactory.addControlElementType(Picto::TargetController::ControllerType(), &Picto::TargetController::NewTargetController);
}


/*	\brief The data used to test all of the different visual elements
 *	
 *	Whenever a new Visual Element is created, it should be added to this funciton to 
 *	test its DataStore functions.
 */
void TestDataStore::TestVisualElement_data()
{

	QTest::addColumn< QSharedPointer<Picto::VisualElement> >("original");

	//////////////////////
	// Test Arrow graphic
	//
	{
	QPoint start(randGen_.randInt(), randGen_.randInt());
	QPoint end(randGen_.randInt(), randGen_.randInt());
	int size = randGen_.randInt()%20;
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> arrow(new Picto::ArrowGraphic(start,end,size,color));

	QTest::newRow("Arrow Graphic") << arrow;
	}

	//////////////////////
	// Test Box graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QRect dimensions(randGen_.randInt(), randGen_.randInt(),randGen_.randInt(), randGen_.randInt());
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> box(new Picto::BoxGraphic(position,dimensions,color));

	QTest::newRow("Box Graphic") << box;
	}

	//////////////////////
	// Test Circle graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	int radius = randGen_.randInt();
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> circle(new Picto::CircleGraphic(position,radius,color));

	QTest::newRow("Circle Graphic") << circle;
	}
	

	//////////////////////
	// Test Ellipse graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QRect dimensions(randGen_.randInt(), randGen_.randInt(),randGen_.randInt(), randGen_.randInt());
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> ellipse(new Picto::EllipseGraphic(position,dimensions,color));

	QTest::newRow("Ellipse Graphic") << ellipse;
	}

	//////////////////////
	// Test Line graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	
	QVector<QPoint> points;
	int numPoints = randGen_.randInt()%8+3;
	for(int i=0; i<numPoints; i++)
	{
		QPoint point(randGen_.randInt(), randGen_.randInt());
		points.push_back(point);
	}

	QSharedPointer<Picto::VisualElement> line(new Picto::LineGraphic(position,points,color));

	QTest::newRow("Line Graphic") << line;
	}

	//////////////////////
	// Test Picture graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QString filename("NotARealFile.bmp");
	QSharedPointer<Picto::VisualElement> pic(new Picto::PictureGraphic(position,filename));

	QTest::newRow("Picture Graphic") << pic;
	}

}

/*!	\brief Tests a single visual element.
 *	The testing consists of:
 *		1. Creating a new visual element with random properties
 *		2. Generating an XML fragment from the visual element
 *		3. Creating a new visual element by deserializing the XML
 *		4. Generating an XML fragment from the new element
 *		5. Confirming that the XML fragments are identical
 */
void TestDataStore::TestVisualElement()
 {
	QFETCH(QSharedPointer<Picto::VisualElement>, original);

	//serialize original
	QByteArray originalByteArr;
	QBuffer originalBuffer(&originalByteArr);
	originalBuffer.open(QBuffer::WriteOnly);
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&originalBuffer));

	xmlWriter->setAutoFormatting(true);
	xmlWriter->writeStartDocument();
	original->toXml(xmlWriter);
	xmlWriter->writeEndDocument();

	originalBuffer.close();

	//create new graphic and deserialize the XML into it
	originalBuffer.open(QBuffer::ReadOnly);
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(&originalBuffer));
	while(xmlReader->name() != "VisualElement")
	{
		xmlReader->readNext();
	}
	QString type = xmlReader->attributes().value("type").toString();
	QSharedPointer<Picto::VisualElement> copy = visualElementFactory_.generateVisualElement(type);
	copy->fromXml(xmlReader);

	//confirm that the xml reader is in the </VisualElement> tag
	QCOMPARE(xmlReader->isEndElement(), true);
	QCOMPARE(xmlReader->name().toString(), QString("VisualElement"));

	originalBuffer.close();

	//reserialize the copied element and confirm that the XML is the same
	QByteArray copyByteArr;
	QBuffer copyBuffer(&copyByteArr);
	copyBuffer.open(QBuffer::WriteOnly);
	xmlWriter->setDevice(&copyBuffer);

	xmlWriter->writeStartDocument();
	copy->toXml(xmlWriter);
	xmlWriter->writeEndDocument();

	QCOMPARE(originalByteArr, copyByteArr);

	

 }



void TestDataStore::TestParameter_data()
{
	//QTest::addColumn< QSharedPointer<Picto::Parameter> >("parameter");

	////////////////////////
	//// Create a boolean parameter
	////
	//Picto::BooleanParameter *boolParam = new Picto::BooleanParameter;
	//boolParam->setName("My boolean Parameter");
	//boolParam->setOrder(31);
	//boolParam->setInitialValue(true);
	//boolParam->setTrueLabel("I'm true!");
	//boolParam->setFalseLabel("I'm false.");
	//QSharedPointer<Picto::Parameter> boolParamPtr(boolParam);

	//QTest::newRow("Boolean Parameter") << boolParamPtr;


	////////////////////////
	//// Create a choice parameter with string choices
	////
	//Picto::ChoiceParameter *choiceParamStrings = new Picto::ChoiceParameter;
	//choiceParamStrings->setName("Choice parameter (strings)");
	//choiceParamStrings->setOrder(42);
	//choiceParamStrings->addChoice("A",QString("Choice A"));
	//choiceParamStrings->addChoice("B",QString("Choice B"));
	//choiceParamStrings->addChoice("C",QString("Choice C"));
	//choiceParamStrings->setInitialValue("B");
	//QSharedPointer<Picto::Parameter> choiceParamStrPtr(choiceParamStrings);

	//QTest::newRow("ChoiceParameter with strings") << choiceParamStrPtr;

	////////////////////////
	//// Create a choice parameter with integer choices
	////
	//Picto::ChoiceParameter *choiceParamInts = new Picto::ChoiceParameter;
	//choiceParamInts->setName("Choice parameter (integers)");
	//choiceParamInts->setOrder(13);
	//choiceParamInts->addChoice("One",1);
	//choiceParamInts->addChoice("Two",2);
	//choiceParamInts->addChoice("Five",5);
	//choiceParamInts->setInitialValue("B");
	//QSharedPointer<Picto::Parameter> choiceParamIntPtr(choiceParamInts);

	//QTest::newRow("ChoiceParameter with integers") << choiceParamIntPtr;

	////////////////////////
	//// Create a choice parameter with color choices
	////
	//Picto::ChoiceParameter *choiceParamColors = new Picto::ChoiceParameter;
	//choiceParamColors->setName("Choice parameter (colors)");
	//choiceParamColors->setOrder(13);
	//choiceParamColors->addChoice("Red",QColor(255,0,0));
	//choiceParamColors->addChoice("Green",QColor(0,255,0));
	//choiceParamColors->addChoice("Blue",QColor(0,0,255));
	//choiceParamColors->setInitialValue("Blue");
	//QSharedPointer<Picto::Parameter> choiceParamColorsPtr(choiceParamColors);

	//QTest::newRow("ChoiceParameter with integers") << choiceParamColorsPtr;

	////////////////////////
	//// Create a range parameter
	////
	//Picto::RangeParameter *rangeParam = new Picto::RangeParameter;
	//rangeParam->setName("Range Parameter");
	//rangeParam->setOrder(13);
	//rangeParam->setIncrement(1);
	//rangeParam->setMax(200);
	//rangeParam->setMin(-50);
	//rangeParam->setInitialValue(45);
	//rangeParam->setUnits("light years");

	//QSharedPointer<Picto::Parameter> rangeParamPtr(rangeParam);

	//QTest::newRow("Range Parameter") << rangeParamPtr;

	////////////////////////
	//// Create a numeric parameter
	////
	//Picto::NumericParameter *numParam = new Picto::NumericParameter;
	//numParam->setName("Numeric Parameter");
	//numParam->setOrder(13);
	//numParam->setUnits("weeks");

	//QSharedPointer<Picto::Parameter> numParamPtr(numParam);

	//QTest::newRow("Numeric Parameter") << numParamPtr;


}

void TestDataStore::TestParameter()
{
	QFETCH(QSharedPointer<Picto::Parameter>, parameter);

	Picto::ParameterFactory paramFact;

	//serialize original
	QByteArray originalByteArr;
	QBuffer originalBuffer(&originalByteArr);
	originalBuffer.open(QBuffer::WriteOnly);
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&originalBuffer));

	xmlWriter->setAutoFormatting(true);
	xmlWriter->writeStartDocument();
	parameter->setName("Test");
	parameter->toXml(xmlWriter);
	xmlWriter->writeEndDocument();

	originalBuffer.close();

	//create new parameter and deserialize the XML into it
	originalBuffer.open(QBuffer::ReadOnly);
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(&originalBuffer));
	while(xmlReader->name() != "Parameter")
	{
		xmlReader->readNext();
	}
	QString type = xmlReader->attributes().value("type").toString();
	QSharedPointer<Picto::Parameter> copy = paramFact.generateParameter(type);
	copy->fromXml(xmlReader);

	//confirm that the xml reader is in the </Parameter> tag
	QCOMPARE(xmlReader->isEndElement(), true);
	QCOMPARE(xmlReader->name().toString(), QString("Parameter"));

	originalBuffer.close();

	//reserialize the copied element and confirm that the XML is the same
	QByteArray copyByteArr;
	QBuffer copyBuffer(&copyByteArr);
	copyBuffer.open(QBuffer::WriteOnly);
	xmlWriter->setDevice(&copyBuffer);

	xmlWriter->writeStartDocument();
	copy->toXml(xmlWriter);
	xmlWriter->writeEndDocument();

	QCOMPARE(originalByteArr, copyByteArr);

}

void TestDataStore::TestParameterContainer()
{
	////Create a Parameter Container & fill it
	//QSharedPointer<Picto::ParameterContainer> originalParameterContainer(new Picto::ParameterContainer);
	//QSharedPointer<Picto::ParameterContainer> copyParameterContainer(new Picto::ParameterContainer);

	//QSharedPointer<Picto::BooleanParameter> boolParam (new Picto::BooleanParameter);
	//boolParam->setName("My boolean Parameter");
	//boolParam->setOrder(31);
	//boolParam->setInitialValue(true);
	//boolParam->setTrueLabel("I'm true!");
	//boolParam->setFalseLabel("I'm false.");
	//originalParameterContainer->addScriptable(boolParam);

	//QSharedPointer<Picto::ChoiceParameter> choiceParamStrings (new Picto::ChoiceParameter);
	//choiceParamStrings->setName("Choice parameter (strings)");
	//choiceParamStrings->setOrder(42);
	//choiceParamStrings->addChoice("A",QString("Choice A"));
	//choiceParamStrings->addChoice("B",QString("Choice B"));
	//choiceParamStrings->addChoice("C",QString("Choice C"));
	//choiceParamStrings->setInitialValue("B");
	//originalParameterContainer->addScriptable(choiceParamStrings);

	//QSharedPointer<Picto::ChoiceParameter> choiceParamInts (new Picto::ChoiceParameter);
	//choiceParamInts->setName("Choice parameter (integers)");
	//choiceParamInts->setOrder(13);
	//choiceParamInts->addChoice("One",1);
	//choiceParamInts->addChoice("Two",2);
	//choiceParamInts->addChoice("Five",5);
	//choiceParamInts->setInitialValue("B");
	//originalParameterContainer->addScriptable(choiceParamInts);

	//QSharedPointer<Picto::ChoiceParameter> choiceParamColors (new Picto::ChoiceParameter);
	//choiceParamColors->setName("Choice parameter (colors)");
	//choiceParamColors->setOrder(13);
	//choiceParamColors->addChoice("Red",QColor(255,0,0));
	//choiceParamColors->addChoice("Green",QColor(0,255,0));
	//choiceParamColors->addChoice("Blue",QColor(0,0,255));
	//choiceParamColors->setInitialValue("Blue");
	//originalParameterContainer->addScriptable(choiceParamColors);

	//QSharedPointer<Picto::RangeParameter> rangeParam (new Picto::RangeParameter);
	//rangeParam->setName("Range Parameter");
	//rangeParam->setOrder(13);
	//rangeParam->setIncrement(1);
	//rangeParam->setMax(200);
	//rangeParam->setMin(-50);
	//rangeParam->setInitialValue(45);
	//rangeParam->setUnits("light years");
	//originalParameterContainer->addScriptable(rangeParam);

	//TestSimpleDataStore(originalParameterContainer,copyParameterContainer,"Parameters");
}


///*!	\brief tests the serialization/deserialization of a PredicateExpression
// *
// *	The following expressions will be tested by serialization/deserialization:
// *		1. Range Parameter > 500
// *		2. Boolean parameter == true
// *		3. RangeParameter < RangeParameter
// */
//void TestDataStore::TestPredicateExpression()
//{
//	QSharedPointer<Picto::PredicateExpression> origPredExp;
//	QSharedPointer<Picto::PredicateExpression> copyPredExp;
//
//	Picto::ParameterContainer paramContainer;
//
//	//Set up a range parameters and a boolean parameter
//	QSharedPointer<Picto::RangeParameter> A(new Picto::RangeParameter);
//	QSharedPointer<Picto::RangeParameter> B(new Picto::RangeParameter);
//	QSharedPointer<Picto::BooleanParameter> C(new Picto::BooleanParameter);
//
//	A->setIncrement(1);
//	A->setDefault(0);
//	A->setMin(5000);
//	A->setMax(-5000);
//	A->setName("Range parameter A");
//	paramContainer.addScriptable(A);
//
//	B->setIncrement(1);
//	B->setDefault(0);
//	B->setMin(5000);
//	B->setMax(-5000);
//	B->setName("Range parameter B");
//	paramContainer.addScriptable(B);
//
//	C->setName("Boolean parameter");
//	C->setDefaultValue(true);
//	C->setTrueLabel("OK");
//	C->setFalseLabel("Fail");
//	paramContainer.addScriptable(C);
//
//	//Test RangeParameter > 500
//	origPredExp = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
//	copyPredExp = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression);
//	origPredExp->setLHS("Range Parameter A");
//	origPredExp->setRHSValue(500);
//	origPredExp->addScriptables(&paramContainer);
//	TestSimpleDataStore(origPredExp, copyPredExp,"Expression");
//
//	//Test Boolean Parameter == true
//	origPredExp = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Equal"));
//	origPredExp->setLHS("Boolean Parameter");
//	origPredExp->setRHSValue(true);
//	origPredExp->addScriptables(&paramContainer);
//	TestSimpleDataStore(origPredExp, copyPredExp,"Expression");
//
//	//Test RangeParameter > 500
//	origPredExp = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than"));
//	origPredExp->setLHS("Range Parameter A");
//	origPredExp->setRHSParam("Range Parameter B");
//	origPredExp->addScriptables(&paramContainer);
//	TestSimpleDataStore(origPredExp,copyPredExp,"Expression");
//}
//
///*!	\brief tests the serialization/deserialization of a CompoundExpression
// *
// *	To fully test the CompoundExpression object, we need to test all of the 
// *	following pieces:
// *		1. Invert left/right
// *		2. Boolean and/or
// *		4. predicateExpression & compoundExpression
// *		5. compoundExpression & predicateExpression
// *		6. compoundExpression & compoundExpression
// *	To keep things simple, we'll use a range parameter and a constant 
// *	for all our expressions
// */
//void TestDataStore::TestCompoundExpression()
//{
//	QSharedPointer<Picto::PredicateExpression> predExprA;
//	QSharedPointer<Picto::PredicateExpression> predExprB;
//	QSharedPointer<Picto::PredicateExpression> predExprC;
//	QSharedPointer<Picto::PredicateExpression> predExprD;
//
//	QSharedPointer<Picto::CompoundExpression> compExprA;
//	QSharedPointer<Picto::CompoundExpression> compExprB;
//	QSharedPointer<Picto::CompoundExpression> origCompoundExpr;
//	QSharedPointer<Picto::CompoundExpression> copyCompoundExpr;
//
//	Picto::ParameterContainer paramContainer;
//
//	//Set up a range parameter
//	QSharedPointer<Picto::RangeParameter> rangeParam(new Picto::RangeParameter);
//	rangeParam->setIncrement(1);
//	rangeParam->setDefault(0);
//	rangeParam->setMin(5000);
//	rangeParam->setMax(-5000);
//	rangeParam->setName("Trial");
//	paramContainer.addScriptable(rangeParam);
//
//	QSharedPointer<Picto::BooleanParameter> boolParam(new Picto::BooleanParameter);
//	boolParam->setName("RepeatTrial");
//	boolParam->setDefaultValue(true);
//	boolParam->setTrueLabel("Repeat");
//	boolParam->setFalseLabel("Don't repeat");
//	paramContainer.addScriptable(boolParam);
//
//
//	////////////////////////////
//	// Test all combination of inversions and boolean operators
//	//
//
//	//set up a pair of simple compound expressions
//	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
//	predExprA->setLHS("Trial");
//	predExprA->setRHSValue(QVariant(50));
//	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than"));
//	predExprB->setLHS("Trial");
//	predExprB->setRHSValue(QVariant(75));
//
//	//set up our compound expression
//	origCompoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
//
//	//test the compound expression with all possible combinations of inversions, and AND/OR
//	for(int i=0; i<8; i++)
//	{
//		if(i/4<1)
//			origCompoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
//		else
//			origCompoundExpr->setOperator(Picto::CompoundExpressionOperator::or);
//
//		switch(i)
//		{
//		case(0):
//			origCompoundExpr->setLHSPredicateExp(predExprA,false);
//			origCompoundExpr->setRHSPredicateExp(predExprB,false);
//			break;
//		case(1):
//			origCompoundExpr->setLHSPredicateExp(predExprA,true);
//			origCompoundExpr->setRHSPredicateExp(predExprB,false);
//			break;
//		case(2):
//			origCompoundExpr->setLHSPredicateExp(predExprA,false);
//			origCompoundExpr->setRHSPredicateExp(predExprB,true);
//			break;
//		case(3):
//			origCompoundExpr->setLHSPredicateExp(predExprA,true);
//			origCompoundExpr->setRHSPredicateExp(predExprB,true);
//			break;
//		}
//		origCompoundExpr->addScriptables(&paramContainer);
//		copyCompoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
//
//		TestSimpleDataStore(origCompoundExpr, copyCompoundExpr, "CompoundExpression");
//	}
//
//	////////////////////////////
//	// Test more complex expressions
//	//
//
//	predExprC = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Equal"));
//	predExprC->setLHS("RepeatTrial");
//	predExprC->setRHSValue(QVariant(false));
//
//	predExprD = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Equal"));
//	predExprD->setLHS("Trial");
//	predExprD->setRHSValue(QVariant(1));
//
//	// Test ((Trial > 50) & (Trial <75)) & (RepeatTrial == true)
//	//----------------------------------------------------------
//	compExprA = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
//	compExprA->setLHSPredicateExp(predExprA);
//	compExprA->setRHSPredicateExp(predExprB);
//	compExprA->setOperator(Picto::CompoundExpressionOperator::and);
//
//	origCompoundExpr->setLHSCompoundExp(compExprA);
//	origCompoundExpr->setRHSPredicateExp(predExprC);
//	origCompoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
//	origCompoundExpr->addScriptables(&paramContainer);
//
//	copyCompoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
//	TestSimpleDataStore(origCompoundExpr, copyCompoundExpr, "CompoundExpression");
//
//	// Test (RepeatTrial == true) & ((Trial > 50) & (Trial <75))
//	//----------------------------------------------------------
//	origCompoundExpr->setLHSPredicateExp(predExprC);
//	origCompoundExpr->setRHSCompoundExp(compExprA);
//	origCompoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
//	origCompoundExpr->addScriptables(&paramContainer);
//
//	copyCompoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
//	TestSimpleDataStore(origCompoundExpr, copyCompoundExpr, "CompoundExpression");
//
//	// Test ((RepeatTrial == true) | (Trial == 1)) & ((Trial > 50) & (Trial <75))
//	//----------------------------------------------------------
//	compExprB = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
//	compExprB->setLHSPredicateExp(predExprC);
//	compExprB->setRHSPredicateExp(predExprD);
//	compExprB->setOperator(Picto::CompoundExpressionOperator::or);
//
//
//	origCompoundExpr->setLHSCompoundExp(compExprB);
//	origCompoundExpr->setRHSCompoundExp(compExprA);
//	origCompoundExpr->addScriptables(&paramContainer);
//	origCompoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
//
//	copyCompoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression);
// 	TestSimpleDataStore(origCompoundExpr, copyCompoundExpr, "CompoundExpression");
//
//
//}

/*!	\brief Tests the serialization/deserialization of a Scene
 *
 */
void TestDataStore::TestScene()
{
	QSharedPointer<Picto::Scene> scene(new Picto::Scene());
	QSharedPointer<Picto::Scene> sceneCopy(new Picto::Scene());

	//Arrow graphic
	QPoint start(randGen_.randInt()%400, randGen_.randInt()%400);
	QPoint end(randGen_.randInt()%400, randGen_.randInt()%400);
	int size = randGen_.randInt()%20;
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> arrow(new Picto::ArrowGraphic(start,end,size,color));

	//Circle graphic
	QPoint position = QPoint(randGen_.randInt()%300, randGen_.randInt()%400);
	int radius = randGen_.randInt()%200;
	color = QColor(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> circle(new Picto::CircleGraphic(position,radius,color));

	//box graphic
	position = QPoint(randGen_.randInt()%300, randGen_.randInt()%400);
	QRect dimensions(0, 0,randGen_.randInt()%50, randGen_.randInt()%200);
	color = QColor(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> box(new Picto::BoxGraphic(position,dimensions,color));

	////layer 1: arrow and circle
	//arrow->setOrder(1);
	//circle->setOrder(1);

	////layer 2: box
	//box->setOrder(2);

	scene->addVisualElement(arrow);
	scene->addVisualElement(circle);
	scene->addVisualElement(box);

	//A scene is no longer a datastore, so we got rid of this
	//TestSimpleDataStore(scene, sceneCopy, "Scene");
}

//! Tests a StateMachine (as well as all the StateMachineElements)
//void TestDataStore::TestStateMachine()
//{
//	///////////////////////////
//	// Results
//	//
//	QSharedPointer<Picto::Reward> passResult(new Picto::Reward());
//	QSharedPointer<Picto::Reward> passResultCopy(new Picto::Reward());
//	passResult->setName("Pass");
//
//	//Test a result
//	TestSimpleDataStore(passResult, passResultCopy, "StateMachineElement");
//
//	//These results will be used later
//	QSharedPointer<Picto::Reward> failResult(new Picto::Reward());
//	failResult->setName("Fail");
//	QSharedPointer<Picto::Reward> passedEmptyResult(new Picto::Reward());
//	passedEmptyResult->setName("Pass Empty");
//
//
//
//	/////////////////////////////
//	// FlowElements
//	//
//	QSharedPointer<Picto::FlowElement> flowElement(new Picto::FlowElement());
//	QSharedPointer<Picto::FlowElement> flowElementCopy(new Picto::FlowElement());
//	flowElement->setName("Trial selection");
//
//	QSharedPointer<Picto::ParameterContainer> parameters(new Picto::ParameterContainer());
//
//	QSharedPointer<Picto::RangeParameter> rangeParam(new Picto::RangeParameter);
//	rangeParam->setName("Trial Number");
//	rangeParam->setOrder(1);
//	rangeParam->setIncrement(1);
//	rangeParam->setMax(500);
//	rangeParam->setMin(100);
//	rangeParam->setDefault(1);
//	rangeParam->setUnits("trial");
//
//	parameters->addScriptable(rangeParam);
//
//	QSharedPointer<Picto::PredicateExpression> predExpr(new Picto::PredicateExpression("Less than"));
//	predExpr->setLHS("Trial Number");
//	predExpr->setRHSValue(50);
//	predExpr->setName("Less than 50");
//	predExpr->setOrder(1);
//	flowElement->addCondition(predExpr);
//	
//	predExpr->setPredicate("Greater than");
//	predExpr->setName("Greater than 50");
//	predExpr->setOrder(2);
//	flowElement->addCondition(predExpr);
//
//	TestSimpleDataStore(flowElement, flowElementCopy, "StateMachineElement");
//
//	////////////////////////////////////////
//	// State
//	//
//	QSharedPointer<Picto::State> state1(new Picto::State);
//	QSharedPointer<Picto::State> state1Copy(new Picto::State);
//	state1->setName("Fixation State");
//
//	//These were removed when this function was removed
//	//state1->addResult(QSharedPointer<Picto::Result>(new Picto::Result("Fixated")));
//	//state1->addResult(QSharedPointer<Picto::Result>(new Picto::Result("Broke Fixation")));
//
//	QSharedPointer<Picto::Scene> scene1(new Picto::Scene());
//	state1->setScene(scene1);
//
//
//	
//
//	//Test the state
//	TestSimpleDataStore(state1, state1Copy, "StateMachineElement");
//
//	//Create a second state for use in our state machine
//	QSharedPointer<Picto::State> state2(new Picto::State);
//	state2->setName("Empty State");
//
//	QSharedPointer<Picto::TestController> controller(new Picto::TestController());
//	state2->addElement(controller);
//
//
//	//////////////////////////////////
//	// State Machine
//	//
//	//	This state machine should be really simple.  It starts with a FlowElement
//	//	that checks if the trial number is > or < 50.  Depending on this result, it
//	//  sends us to a Fixation trial or an empty trial.  The state machine has two
//	//	possible results: Pass, Fail.
//	QSharedPointer<Picto::StateMachine> stateMachine(new Picto::StateMachine);
//	QSharedPointer<Picto::StateMachine> stateMachineCopy(new Picto::StateMachine);
//	stateMachine->setLevel(Picto::StateMachineLevel::Stage);
//	stateMachine->setName("Stage 1");
//
//	stateMachine->addScriptable(rangeParam);
//	stateMachine->addElement(passResult);
//	stateMachine->addElement(flowElement);
//	stateMachine->addElement(state1);
//	stateMachine->addElement(state2);
//
//	stateMachine->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Trial selection","Less than 50","Fixation State")));
//	stateMachine->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Fixation State","Fixated","Pass")));
//	stateMachine->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Empty State","Success","Pass")));
//
//	stateMachine->setInitialElement("Trial selection");
//
//	TestSimpleDataStore(stateMachine, stateMachineCopy, "StateMachine");
//
//	//The reason for this odd arrangement is due to the fact that the TestSimpleDataStore function requires
//	//the two XML fragments to be identical, but since we are using lists and maps, the ordering sometimes gets reversed.
//	//So we don't test a full state machine
//	stateMachine->addElement(failResult);
//	stateMachine->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Trial selection","Greater than 50","Empty State")));
//	stateMachine->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Fixation State","Broke Fixation","Fail")));
//	//QCOMPARE(stateMachine->validateStateMachine(),true);//THIS WAS REMOVED BECAUSE VALIDATION IS IN VALIDATE OBJECT NOW, BUT THIS MAKES THE TEST INOPERATIVE
//
//	//Testing the ability to add substages
//	//QSharedPointer<Picto::StateMachine> stateMachine2(new Picto::StateMachine);
//	//stateMachine2->setLevel("SubStage");
//	//stateMachine2->setName("SubStage 1");
//	//stateMachine2->addElement(passResult);
//	//stateMachine2->addElement(failResult);
//	//stateMachine2->addElement(state1);
//	//stateMachine2->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Fixation State","Fixated","Pass")));
//	//stateMachine2->addTransition(QSharedPointer<Picto::Transition>(new Picto::Transition("Fixation State","Broke Fixation","Fail")));
//	//stateMachine2->setInitialElement("Fixation State");
//
//	//stateMachine->addElement(stateMachine2);
//
//	//Output the StateMachine to file so that we can reuse it later.
//	QFile file("StateMachineTest1.xml");
//	if(!file.open(QIODevice::WriteOnly))
//		return;
//	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&file));
//
//	xmlWriter->setAutoFormatting(true);
//	xmlWriter->writeStartDocument();
//	stateMachine->toXml(xmlWriter);
//	xmlWriter->writeEndDocument();
//
//	file.close();
//
//}


//! This function tests a simple DataStore object
void TestDataStore::TestSimpleDataStore(QSharedPointer<Picto::DataStore> original, QSharedPointer<Picto::DataStore> copy, QString startElementName)
{
	QByteArray originalByteArr;
	QByteArray copyByteArr;

	//Serialize it
	QBuffer originalBuffer(&originalByteArr);
	originalBuffer.open(QBuffer::WriteOnly);
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&originalBuffer));

	xmlWriter->setAutoFormatting(true);
	xmlWriter->writeStartDocument();
	original->toXml(xmlWriter);
	xmlWriter->writeEndDocument();

	originalBuffer.close();

	//Deserialize it
	originalBuffer.open(QBuffer::ReadOnly);
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(&originalBuffer));
	while(xmlReader->name() != startElementName)
	{
		xmlReader->readNext();
	}
	copy->fromXml(xmlReader);

	QCOMPARE(xmlReader->isEndElement(), true);
	QCOMPARE(xmlReader->name().toString(), QString(startElementName));

	originalBuffer.close();

	//Reserialize it
	QBuffer copyBuffer(&copyByteArr);
	copyBuffer.open(QBuffer::WriteOnly);
	xmlWriter->setDevice(&copyBuffer);

	xmlWriter->writeStartDocument();
	copy->toXml(xmlWriter);
	xmlWriter->writeEndDocument();

	//Compare results
	QCOMPARE(originalByteArr, copyByteArr);
}



