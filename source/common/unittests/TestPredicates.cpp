#include "TestPredicates.h"

#include "../statemachine/Predicate.h"
#include "../statemachine/PredicateExpression.h"
#include "../statemachine/CompoundExpression.h"
#include "../statemachine/PredicateGreaterThan.h"
#include "../statemachine/PredicateGreaterThanEq.h"
#include "../statemachine/PredicateLessThan.h"
#include "../statemachine/PredicateLessThanEq.h"
#include "../statemachine/PredicateEqual.h"
#include "../statemachine/PredicateNotEqual.h"

#include "../parameter/RangeParameter.h"
#include "../parameter/BooleanParameter.h"
#include "../parameter/ChoiceParameter.h"

#include "../parameter/ParameterContainer.h"


TestPredicates::TestPredicates()
{
	randGen.seed();
}


/*!	\brief Tests that single predicates are evaluating correctly
 *
 *	This test runs through all of the existing predicates and tests
 *	their "evaluate" function.  Since the comparison operators are fully
 *	tested in "TestParameters", we're going to use only RageParameters
 *	for this test.  However, we do need to test evaluation of the predicate
 *	when comparing two parameters, and when comparing a paramter to a constant
 */
void TestPredicates::TestPredicateEvaluation()
{
	Picto::Predicate *pred;
	Picto::RangeParameter A,B;
	QVariant AConst, BConst;

	int numTests = 100;
	double maxValue = 5000;
	double minValue = -5000;

	A.setIncrement(1);
	A.setDefault(0);
	A.setMin(minValue);
	A.setMax(maxValue);
	B.setIncrement(1);
	B.setDefault(0);
	B.setMin(minValue);
	B.setMax(maxValue);


	////////////////////////////
	// PredicateGreaterThan
	//
	pred = new Picto::PredicateGreaterThan();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A.setValue(randGen.randInt(maxValue-minValue) + minValue);
		B.setValue(randGen.randInt(maxValue-minValue) + minValue);
		AConst = A.getValue().toInt();
		BConst = B.getValue().toInt();

		//test all four possible combinations using both parameters
		QCOMPARE(pred->evaluate(A,B), (A>B));
		QCOMPARE(pred->evaluate(B,A), (B>A));
		QCOMPARE(pred->evaluate(A,A), (A>A));
		QCOMPARE(pred->evaluate(B,B), (B>B));

		//test with a parameter and a constant
		QCOMPARE(pred->evaluate(A,BConst), (A>BConst));
		QCOMPARE(pred->evaluate(B,AConst), (B>AConst));
		QCOMPARE(pred->evaluate(A,AConst), (A>AConst));
		QCOMPARE(pred->evaluate(B,BConst), (B>BConst));
	}

	delete pred;

	////////////////////////////
	// PredicateGreaterThanEq
	//
	pred = new Picto::PredicateGreaterThanEq();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A.setValue(randGen.randInt(maxValue-minValue) + minValue);
		B.setValue(randGen.randInt(maxValue-minValue) + minValue);
		AConst = A.getValue().toInt();
		BConst = B.getValue().toInt();

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A>=B));
		QCOMPARE(pred->evaluate(B,A), (B>=A));
		QCOMPARE(pred->evaluate(A,A), (A>=A));
		QCOMPARE(pred->evaluate(B,B), (B>=B));

		//test with a parameter and a constant
		QCOMPARE(pred->evaluate(A,BConst), (A>=BConst));
		QCOMPARE(pred->evaluate(B,AConst), (B>=AConst));
		QCOMPARE(pred->evaluate(A,AConst), (A>=AConst));
		QCOMPARE(pred->evaluate(B,BConst), (B>=BConst));
	}

	delete pred;

	////////////////////////////
	// PredicateLessThan
	//
	pred = new Picto::PredicateLessThan();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A.setValue(randGen.randInt(maxValue-minValue) + minValue);
		B.setValue(randGen.randInt(maxValue-minValue) + minValue);
		AConst = A.getValue().toInt();
		BConst = B.getValue().toInt();

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A<B));
		QCOMPARE(pred->evaluate(B,A), (B<A));
		QCOMPARE(pred->evaluate(A,A), (A<A));
		QCOMPARE(pred->evaluate(B,B), (B<B));

		//test with a parameter and a constant
		QCOMPARE(pred->evaluate(A,BConst), (A<BConst));
		QCOMPARE(pred->evaluate(B,AConst), (B<AConst));
		QCOMPARE(pred->evaluate(A,AConst), (A<AConst));
		QCOMPARE(pred->evaluate(B,BConst), (B<BConst));
	}

	delete pred;

	////////////////////////////
	// PredicateLessThanEq
	//
	pred = new Picto::PredicateLessThanEq();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A.setValue(randGen.randInt(maxValue-minValue) + minValue);
		B.setValue(randGen.randInt(maxValue-minValue) + minValue);
		AConst = A.getValue().toInt();
		BConst = B.getValue().toInt();

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A<=B));
		QCOMPARE(pred->evaluate(B,A), (B<=A));
		QCOMPARE(pred->evaluate(A,A), (A<=A));
		QCOMPARE(pred->evaluate(B,B), (B<=B));

		//test with a parameter and a constant
		QCOMPARE(pred->evaluate(A,BConst), (A<=BConst));
		QCOMPARE(pred->evaluate(B,AConst), (B<=AConst));
		QCOMPARE(pred->evaluate(A,AConst), (A<=AConst));
		QCOMPARE(pred->evaluate(B,BConst), (B<=BConst));
	}

	delete pred;

	////////////////////////////
	// PredicateEqual
	//
	pred = new Picto::PredicateEqual();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A.setValue(randGen.randInt(maxValue-minValue) + minValue);
		B.setValue(randGen.randInt(maxValue-minValue) + minValue);
		AConst = A.getValue().toInt();
		BConst = B.getValue().toInt();

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A==B));
		QCOMPARE(pred->evaluate(B,A), (B==A));
		QCOMPARE(pred->evaluate(A,A), (A==A));
		QCOMPARE(pred->evaluate(B,B), (B==B));

		//test with a parameter and a constant
		QCOMPARE(pred->evaluate(A,BConst), (A==BConst));
		QCOMPARE(pred->evaluate(B,AConst), (B==AConst));
		QCOMPARE(pred->evaluate(A,AConst), (A==AConst));
		QCOMPARE(pred->evaluate(B,BConst), (B==BConst));
	}

	delete pred;

	////////////////////////////
	// PredicateNotEqual
	//
	pred = new Picto::PredicateNotEqual();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A.setValue(randGen.randInt(maxValue-minValue) + minValue);
		B.setValue(randGen.randInt(maxValue-minValue) + minValue);
		AConst = A.getValue().toInt();
		BConst = B.getValue().toInt();

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A!=B));
		QCOMPARE(pred->evaluate(B,A), (B!=A));
		QCOMPARE(pred->evaluate(A,A), (A!=A));
		QCOMPARE(pred->evaluate(B,B), (B!=B));

		//test with a parameter and a constant
		QCOMPARE(pred->evaluate(A,BConst), (A!=BConst));
		QCOMPARE(pred->evaluate(B,AConst), (B!=AConst));
		QCOMPARE(pred->evaluate(A,AConst), (A!=AConst));
		QCOMPARE(pred->evaluate(B,BConst), (B!=BConst));
	}

	delete pred;


}

/*! \brief Tests the various output functions of the pedicate
 *
 *	Each predicate has 2 output functions: toString() and toQImage()
 *	Testing the String output function is trivial, but since I don't 
 *	have the decade neccessary for writing an image recognition and
 *	evaluation tool, testing the toQImage() functions has to be done
 *	manually.  For each predicate, a bmp file will be be generated in
 *	the program's directory.  It is then the user's responsibility to 
 *	confirm that the bmp images are correct.
 */
void TestPredicates::TestPredicateOutputs()
{
	Picto::Predicate *pred;
	QImage image;

	////////////////////////////
	// PredicateGreaterThan
	//
	pred = new Picto::PredicateGreaterThan();
	QCOMPARE(pred->toString(), QString(">"));

	image = pred->toQImage();
	image.save("PredicateGreaterThan.bmp","BMP");
	delete pred;

	////////////////////////////
	// PredicateGreaterThanEq
	//
	pred = new Picto::PredicateGreaterThanEq();
	QCOMPARE(pred->toString(), QString(">="));

	image = pred->toQImage();
	image.save("PredicateGreaterThanEq.bmp","BMP");
	delete pred;

	////////////////////////////
	// PredicateLessThan
	//
	pred = new Picto::PredicateLessThan();
	QCOMPARE(pred->toString(), QString("<"));

	image = pred->toQImage();
	image.save("PredicateLessThan.bmp","BMP");
	delete pred;

	////////////////////////////
	// PredicateLessThanEq
	//
	pred = new Picto::PredicateLessThanEq();
	QCOMPARE(pred->toString(), QString("<="));

	image = pred->toQImage();
	image.save("PredicateLessThanEq.bmp","BMP");
	delete pred;

	////////////////////////////
	// PredicateEqual
	//
	pred = new Picto::PredicateEqual();
	QCOMPARE(pred->toString(), QString("=="));

	image = pred->toQImage();
	image.save("PredicateEqual.bmp","BMP");
	delete pred;

	////////////////////////////
	// PredicateNotEqual
	//
	pred = new Picto::PredicateNotEqual();
	QCOMPARE(pred->toString(), QString("!="));

	image = pred->toQImage();
	image.save("PredicateNotEqual.bmp","BMP");
	delete pred;

	QWARN("Don't forget to check the image files to confirm output");
}

/*!	\brief Tests the evaluation of predicate expressions 
 *
 *	Throughly testing the evaluation of predicate expressions is actually
 *	really simple.  Since the predicate evaluation is already tested, I 
 *	can just test pick one of the predicates and use it.  Just for fun,
 *	I'm going to test with a BooleanParameter and equals, as well as the more
 *	thorough test with a RangeParameter
 */
void TestPredicates::TestPredicateExpressionEvaluation()
{
	Picto::PredicateExpression *predExp;

	Picto::ParameterContainer paramContainer;

	///////////////////////////////
	//  Test 1 - RangeParameter
	//
	predExp = new Picto::PredicateExpression("Greater than");

	QSharedPointer<Picto::RangeParameter> A(new Picto::RangeParameter);
	QSharedPointer<Picto::RangeParameter> B(new Picto::RangeParameter);
	QVariant AConst, BConst;

	int numTests = 100;
	double maxValue = 5000;
	double minValue = -5000;

	A->setIncrement(1);
	A->setDefault(0);
	A->setMin(minValue);
	A->setMax(maxValue);
	A->setName("RangeParameterA");
	B->setIncrement(1);
	B->setDefault(0);
	B->setMin(minValue);
	B->setMax(maxValue);
	B->setName("RangeParameterB");
	paramContainer.addScriptable(A);
	paramContainer.addScriptable(B);
	predExp->addScriptables(&paramContainer);

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		int Avalue = randGen.randInt(maxValue-minValue) + minValue;
		int Bvalue = randGen.randInt(maxValue-minValue) + minValue;
		A->setValue(Avalue);
		B->setValue(Bvalue);
		AConst = A->getValue().toInt();
		BConst = B->getValue().toInt();

		//test all four combinations using Parameters on both sides
		predExp->setLHS("RangeParameterA");
		predExp->setRHSParam("RangeParameterB");
		QCOMPARE(predExp->evaluate(),(Avalue>Bvalue));

		predExp->setLHS("RangeParameterB");
		predExp->setRHSParam("RangeParameterA");
		QCOMPARE(predExp->evaluate(),(Bvalue>Avalue));

		predExp->setLHS("RangeParameterA");
		predExp->setRHSParam("RangeParameterA");
		QCOMPARE(predExp->evaluate(),(Avalue>Avalue));

		predExp->setLHS("RangeParameterB");
		predExp->setRHSParam("RangeParameterB");
		QCOMPARE(predExp->evaluate(),(Bvalue>Bvalue));

		//test all four combinations using constants on right sides
		predExp->setLHS("RangeParameterA");
		predExp->setRHSValue(BConst);
		QCOMPARE(predExp->evaluate(),(Avalue>Bvalue));

		predExp->setLHS("RangeParameterB");
		predExp->setRHSValue(AConst);
		QCOMPARE(predExp->evaluate(),(Bvalue>Avalue));

		predExp->setLHS("RangeParameterA");
		predExp->setRHSValue(AConst);
		QCOMPARE(predExp->evaluate(),(Avalue>Avalue));

		predExp->setLHS("RangeParameterB");
		predExp->setRHSValue(BConst);
		QCOMPARE(predExp->evaluate(),(Bvalue>Bvalue));
	}

	delete predExp;

	///////////////////////////////
	//  Test 2 - BooleanParameter
	//
	predExp = new Picto::PredicateExpression("Equal");

	QSharedPointer<Picto::BooleanParameter> C(new Picto::BooleanParameter);
	QSharedPointer<Picto::BooleanParameter> D(new Picto::BooleanParameter);


	C->setValue(true);
	D->setValue(false);
	C->setName("BooleanParameterC");
	D->setName("BooleanParameterD");

	paramContainer.addScriptable(C);
	paramContainer.addScriptable(D);

	predExp->addScriptables(&paramContainer);

	//test all the combinations of parameters
	predExp->setLHS("BooleanParameterC");
	predExp->setRHSParam("BooleanParameterD");
	QCOMPARE(predExp->evaluate(), false);

	predExp->setLHS("BooleanParameterD");
	predExp->setRHSParam("BooleanParameterC");
	QCOMPARE(predExp->evaluate(), false);

	predExp->setLHS("BooleanParameterC");
	predExp->setRHSParam("BooleanParameterC");
	QCOMPARE(predExp->evaluate(), true);

	predExp->setLHS("BooleanParameterD");
	predExp->setRHSParam("BooleanParameterD");
	QCOMPARE(predExp->evaluate(), true);

	//test all the combinations with constants
	predExp->setLHS("BooleanParameterD");
	predExp->setRHSValue(QVariant(true));
	QCOMPARE(predExp->evaluate(), false);

	predExp->setLHS("BooleanParameterD");
	predExp->setRHSValue(QVariant(false));
	QCOMPARE(predExp->evaluate(), true);

	delete predExp;
}

/*!	\brief This tests the output functionality (both text based and graphical)
 *
 *	Each predicate expression has 4 possible output modes (depending on whether
 *	we choose to display the name or the value).  All of these modes need to be
 *	tested in both text output (toString) and graphical output (toQImage).  Again,
 *	we'll output the images to a file for later checking.
 */
void TestPredicates::TestPredicateExpressionOutputs()
{
	Picto::ParameterContainer paramContainer;

	Picto::PredicateExpression *predExp;
	Picto::Predicate *pred;

	QSharedPointer<Picto::ChoiceParameter> param1(new Picto::ChoiceParameter);
	QSharedPointer<Picto::ChoiceParameter> param2(new Picto::ChoiceParameter);
	param1->setName("Parameter 1");
	param1->addChoice("One", 1);
	param1->addChoice("Two", 2);
	param1->setValue("One");
	param2->setName("Parameter 2");
	param2->addChoice("One", 1);
	param2->addChoice("Two", 2);
	param2->setValue("Two");

	paramContainer.addScriptable(param1);
	paramContainer.addScriptable(param2);

	////////////////////////////////
	//Test the text output with parameters on both sides
	//

	pred = new Picto::PredicateGreaterThan();
	predExp = new Picto::PredicateExpression("Greater than");
	predExp->setLHS("Parameter 1");
	predExp->setRHSParam("Parameter 2");
	predExp->addScriptables(&paramContainer);

	QCOMPARE(predExp->toString(false,false),QString("1 > 2"));
	QCOMPARE(predExp->toString(true,false),QString("Parameter 1 > 2"));
	QCOMPARE(predExp->toString(false,true),QString("1 > Parameter 2"));
	QCOMPARE(predExp->toString(true,true),QString("Parameter 1 > Parameter 2"));
	
	////////////////////////////////
	//Test the text output with a constant on one sides
	//
	predExp->setRHSValue(3);
	QCOMPARE(predExp->toString(false,false),QString("1 > 3"));
	QCOMPARE(predExp->toString(true,false),QString("Parameter 1 > 3"));
	QCOMPARE(predExp->toString(false,true),QString("1 > 3"));
	QCOMPARE(predExp->toString(true,true),QString("Parameter 1 > 3"));


	////////////////////////////////
	//Test the image output
	//
	QImage testImage;
	predExp->setRHSParam("Parameter 2");

	testImage = predExp->toQImage(false, false);
	testImage.save("PredicateExpression1.bmp","BMP");

	testImage = predExp->toQImage(true, false);
	testImage.save("PredicateExpression2.bmp","BMP");

	testImage = predExp->toQImage(false, true);
	testImage.save("PredicateExpression3.bmp","BMP");

	testImage = predExp->toQImage(true, true);
	testImage.save("PredicateExpression4.bmp","BMP");

	////////////////////////////////
	//Test the text output with a constant on one sides
	//
	predExp->setRHSValue(3);
	testImage = predExp->toQImage(true, true);
	testImage.save("PredicateExpression5.bmp","BMP");

	delete predExp;
	delete pred;

	QWARN("Don't forget to check the image files to confirm output");

}

/*!	\brief Tests the CompoundExpression objects evaluation function
 *
 *	Coumpound expressions can range from simple (A>B) & (B>C) to infinitely
 *	complex.  These test cases cover the basics and some of the more complex
 *	cases.  Since the predicates and predicate expressions have already 
 *	been tested, we'll keep that part simple, and stick to really abusing
 *	the compound expression object.
 *
 *	The following test cases will be used:
 *		1. expA AND/OR expB (fully tested)
 *		2. (expA AND expB) OR expC
 *		3. (expA AND expB) OR (expC AND expD)
 *
 *	For each test case, multiple combinations may be tested (AND/OR, inverting
 *	one or both sides).
 */
void TestPredicates::TestCompoundExpressionEvaluation()
{
	Picto::ParameterContainer paramContainer;

	QSharedPointer<Picto::CompoundExpression> compoundExpr;
	QSharedPointer<Picto::CompoundExpression> compoundExprA;
	QSharedPointer<Picto::CompoundExpression> compoundExprB;
	QSharedPointer<Picto::PredicateExpression> predExprA;
	QSharedPointer<Picto::PredicateExpression> predExprB;
	QSharedPointer<Picto::PredicateExpression> predExprC;
	QSharedPointer<Picto::PredicateExpression> predExprD;

	int numTests = 10;
	double maxValue = 5000;
	double minValue = -5000;

	QStringList rangeParameterNames;
	rangeParameterNames.push_back("Range Parameter A Left");
	rangeParameterNames.push_back("Range Parameter A Right");
	rangeParameterNames.push_back("Range Parameter B Left");
	rangeParameterNames.push_back("Range Parameter B Right");
	rangeParameterNames.push_back("Range Parameter C Left");
	rangeParameterNames.push_back("Range Parameter C Right");
	rangeParameterNames.push_back("Range Parameter D Left");
	rangeParameterNames.push_back("Range Parameter D Right");

	foreach(QString name, rangeParameterNames)
	{
		QSharedPointer<Picto::RangeParameter> range(new Picto::RangeParameter);
		range->setIncrement(1);
		range->setDefault(0);
		range->setMin(minValue);
		range->setMax(maxValue);
		range->setName(name);

		paramContainer.addScriptable(range);
	}


	/////////////////////
	//  Case 1        
	//
	//	This case tests the basic functionality of a compound expression made
	//	up of two predicate expressions.  The full range of possibilities are
	//	tested (and, or, invert either or both sides of the expression).  Random 
	//	values are used to really stretch the object.

	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprA->setLHS("Range Parameter A Left");
	predExprA->setRHSParam("Range Parameter A Right");
	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprB->setLHS("Range Parameter B Left");
	predExprB->setRHSParam("Range Parameter B Right");
	compoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression());
	compoundExpr->setLHSPredicateExp(predExprA, false);
	compoundExpr->setRHSPredicateExp(predExprB, false);
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
	compoundExpr->addScriptables(&paramContainer);



	//Test list
	//	1.test A & B
	//	2.test !A & B
	//	3.test A & !B
	//	4.test !A & !B
	//	5.test A | B
	//	6.test !A | B
	//	7.test A | !B
	//	8.test !A | !B
	//----------
	for(int test=1; test<=8; test++)
	{
		//set the operator to AND for tests 1-4, and OR for tests 5-8
		if(test<=4)
			compoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
		else
			compoundExpr->setOperator(Picto::CompoundExpressionOperator::or);

		//deal with the different inversions
		if(test==1 || test ==5)
		{
			compoundExpr->setLHSPredicateExp(predExprA,false);
			compoundExpr->setRHSPredicateExp(predExprB,false);
		}
		else if(test==2 || test ==6)
		{
			compoundExpr->setLHSPredicateExp(predExprA,true);
			compoundExpr->setRHSPredicateExp(predExprB,false);
		}
		else if(test==3 || test ==7)
		{
			compoundExpr->setLHSPredicateExp(predExprA,false);
			compoundExpr->setRHSPredicateExp(predExprB,true);
		}
		else if(test==4 || test ==8)
		{
			compoundExpr->setLHSPredicateExp(predExprA,true);
			compoundExpr->setRHSPredicateExp(predExprB,true);
		}

		for(int i=0; i<numTests; i++)
		{
			//generate random values
			int ALeftVal = randGen.randInt(maxValue-minValue) + minValue;
			int ARightVal = randGen.randInt(maxValue-minValue) + minValue;
			int BLeftVal = randGen.randInt(maxValue-minValue) + minValue;
			int BRightVal = randGen.randInt(maxValue-minValue) + minValue;
			paramContainer.getParameter("Range Parameter A Left")->setValue(ALeftVal);
			paramContainer.getParameter("Range Parameter A Right")->setValue(ARightVal);
			paramContainer.getParameter("Range Parameter B Left")->setValue(BLeftVal);
			paramContainer.getParameter("Range Parameter B Right")->setValue(BRightVal);

			if(test==1)
				QCOMPARE(compoundExpr->evaluate(), (ALeftVal>ARightVal) && (BLeftVal > BRightVal));
			if(test==2)
				QCOMPARE(compoundExpr->evaluate(), !(ALeftVal>ARightVal) && (BLeftVal > BRightVal));
			if(test==3)
				QCOMPARE(compoundExpr->evaluate(), (ALeftVal>ARightVal) && !(BLeftVal > BRightVal));
			if(test==4)
				QCOMPARE(compoundExpr->evaluate(), !(ALeftVal>ARightVal) && !(BLeftVal > BRightVal));
			if(test==5)
				QCOMPARE(compoundExpr->evaluate(), (ALeftVal>ARightVal) || (BLeftVal > BRightVal));
			if(test==6)
				QCOMPARE(compoundExpr->evaluate(), !(ALeftVal>ARightVal) || (BLeftVal > BRightVal));
			if(test==7)
				QCOMPARE(compoundExpr->evaluate(), (ALeftVal>ARightVal) || !(BLeftVal > BRightVal));
			if(test==8)
				QCOMPARE(compoundExpr->evaluate(), !(ALeftVal>ARightVal) || !(BLeftVal > BRightVal));
		}
	}


	/////////////////////
	//  Case 2        
	//
	//	This case tests the basic functionality of a compound expression made
	//	up of a predicate expressions and a coupound expression.  The expression 
	//	being used is:
	//		(expA AND expB) OR expC
	//	Since we already did a full test, it seems that we don't need to really 
	//	play with all the different variations (also, looking at the code, the
	//	inversion and combination of the results occur after evaluation, so those
	//	code paths were already tested

	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprA->setLHS("Range Parameter A Left");
	predExprA->setRHSParam("Range Parameter A Right");
	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprB->setLHS("Range Parameter B Left");
	predExprB->setRHSParam("Range Parameter B Right");
	predExprC = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprC->setLHS("Range Parameter C Left");
	predExprC->setRHSParam("Range Parameter C Right");

	compoundExprA= QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression());
	compoundExprA->setLHSPredicateExp(predExprA,false);
	compoundExprA->setRHSPredicateExp(predExprB,false);
	compoundExprA->setOperator(Picto::CompoundExpressionOperator::and);

	compoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression());
	compoundExpr->setLHSCompoundExp(compoundExprA,false);
	compoundExpr->setRHSPredicateExp(predExprC,false);
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::or);
	compoundExpr->addScriptables(&paramContainer);


	for(int i=0; i<numTests; i++)
	{
		//generate random values
		int ALeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int ARightVal = randGen.randInt(maxValue-minValue) + minValue;
		int BLeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int BRightVal = randGen.randInt(maxValue-minValue) + minValue;
		int CLeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int CRightVal = randGen.randInt(maxValue-minValue) + minValue;

		paramContainer.getParameter("Range Parameter A Left")->setValue(ALeftVal);
		paramContainer.getParameter("Range Parameter A Right")->setValue(ARightVal);
		paramContainer.getParameter("Range Parameter B Left")->setValue(BLeftVal);
		paramContainer.getParameter("Range Parameter B Right")->setValue(BRightVal);
		paramContainer.getParameter("Range Parameter C Left")->setValue(CLeftVal);
		paramContainer.getParameter("Range Parameter C Right")->setValue(CRightVal);

		QCOMPARE(compoundExpr->evaluate(), ((ALeftVal > ARightVal) && (BLeftVal > BRightVal)) || (CLeftVal > CRightVal));
	}

	/////////////////////
	//  Case 3        
	//
	//	This case tests the basic functionality of a compound expression made
	//	up of a predicate expressions and a coupound expression.  The expression 
	//	being used is:
	//		(expA AND expB) OR (expC AND expD)

	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprA->setLHS("Range Parameter A Left");
	predExprA->setRHSParam("Range Parameter A Right");
	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than"));
	predExprB->setLHS("Range Parameter B Left");
	predExprB->setRHSParam("Range Parameter B Right");
	predExprC = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than or equal"));
	predExprC->setLHS("Range Parameter C Left");
	predExprC->setRHSParam("Range Parameter C Right");
	predExprD = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than or equal"));
	predExprD->setLHS("Range Parameter D Left");
	predExprD->setRHSParam("Range Parameter D Right");

	compoundExprA= QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression());
	compoundExprA->setLHSPredicateExp(predExprA,false);
	compoundExprA->setRHSPredicateExp(predExprB,false);
	compoundExprA->setOperator(Picto::CompoundExpressionOperator::and);

	compoundExprB = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression());
	compoundExprB->setLHSPredicateExp(predExprC,false);
	compoundExprB->setRHSPredicateExp(predExprD,false);
	compoundExprB->setOperator(Picto::CompoundExpressionOperator::and);

	compoundExpr = QSharedPointer<Picto::CompoundExpression>(new Picto::CompoundExpression());
	compoundExpr->setLHSCompoundExp(compoundExprA,false);
	compoundExpr->setRHSCompoundExp(compoundExprB,false);
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::or);
	compoundExpr->addScriptables(&paramContainer);


	for(int i=0; i<numTests; i++)
	{
		//generate random values
		int ALeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int ARightVal = randGen.randInt(maxValue-minValue) + minValue;
		int BLeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int BRightVal = randGen.randInt(maxValue-minValue) + minValue;
		int CLeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int CRightVal = randGen.randInt(maxValue-minValue) + minValue;
		int DLeftVal = randGen.randInt(maxValue-minValue) + minValue;
		int DRightVal = randGen.randInt(maxValue-minValue) + minValue;

		paramContainer.getParameter("Range Parameter A Left")->setValue(ALeftVal);
		paramContainer.getParameter("Range Parameter A Right")->setValue(ARightVal);
		paramContainer.getParameter("Range Parameter B Left")->setValue(BLeftVal);
		paramContainer.getParameter("Range Parameter B Right")->setValue(BRightVal);
		paramContainer.getParameter("Range Parameter C Left")->setValue(CLeftVal);
		paramContainer.getParameter("Range Parameter C Right")->setValue(CRightVal);
		paramContainer.getParameter("Range Parameter D Left")->setValue(DLeftVal);
		paramContainer.getParameter("Range Parameter D Right")->setValue(DRightVal);

		QCOMPARE(compoundExpr->evaluate(), ((ALeftVal > ARightVal) && (BLeftVal < BRightVal)) || ((CLeftVal >= CRightVal) && (DLeftVal <= DRightVal)));
	}
}


/*! \brief This tests the output functions (toString() and toQImage()) of the CompoundExpression object
 *
 *	Since there are literally an infinite number of possible compound expressions,
 *	testing all of them is impossible.  However, this test will attempt to cover
 *	the more common cases.
 *
 *	For each compound expression, we will output it both as a string and to a jpg
 *	file.  We will also test all four possibilities for displaying the names or values.
 *
 *	The following expressions will be used:
 *		1. expA & expB
 *				This will be tested with AND, OR, and inversions, but
 *				only displaying the values
 *		2. expA & expB
 *				This will be tested with all four possibilities for name display
 *		3. (expA & expB) | expC
 *				This will be tested with only names and only values
 *		4. (expA & expB) | (expC & expD)
 *				This will be tested with only names and only values
 *
 */
void TestPredicates::TestCompoundExpressionOutputs()
{
	Picto::ParameterContainer paramContainer;

	QSharedPointer<Picto::CompoundExpression> compoundExpr(new Picto::CompoundExpression());
	QSharedPointer<Picto::CompoundExpression> compoundExprA(new Picto::CompoundExpression());
	QSharedPointer<Picto::CompoundExpression> compoundExprB(new Picto::CompoundExpression());
	QSharedPointer<Picto::PredicateExpression> predExprA;
	QSharedPointer<Picto::PredicateExpression> predExprB;
	QSharedPointer<Picto::PredicateExpression> predExprC;
	QSharedPointer<Picto::PredicateExpression> predExprD;

	QImage testImage;

	double maxValue = 5000;
	double minValue = -5000;

	QStringList rangeParameterNames;
	rangeParameterNames.push_back("A Left");
	rangeParameterNames.push_back("A Right");
	rangeParameterNames.push_back("B Left");
	rangeParameterNames.push_back("B Right");
	rangeParameterNames.push_back("C Left");
	rangeParameterNames.push_back("C Right");
	rangeParameterNames.push_back("D Left");
	rangeParameterNames.push_back("D Right");

	foreach(QString name, rangeParameterNames)
	{
		QSharedPointer<Picto::RangeParameter> range(new Picto::RangeParameter);
		range->setIncrement(1);
		range->setDefault(0);
		range->setMin(minValue);
		range->setMax(maxValue);
		range->setName(name);
		range->setValue(42);
		paramContainer.addScriptable(range);
	}


	/////////////////////
	//  Case 1        
	//
	//	This case tests the oupout functionality of a compound expression made
	//	up of two predicate expressions.  The full range of possibilities are
	//	tested (and, or, invert either or both sides of the expression) with the
	//	expression, but the display options are limited to names on the left and
	//	values on the right

	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Equal"));
	predExprA->setLHS("A Left");
	predExprA->setRHSParam("A Right");
	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than"));
	predExprB->setLHS("B Left");
	predExprB->setRHSParam("B Right");

	compoundExpr->setLHSPredicateExp(predExprA);
	compoundExpr->setRHSPredicateExp(predExprB);
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
	compoundExpr->addScriptables(&paramContainer);


	//Test list
	//	1.test A & B
	//	2.test !A & B
	//	3.test A & !B
	//	4.test !A & !B
	//	5.test A | B
	//	6.test !A | B
	//	7.test A | !B
	//	8.test !A | !B
	//----------
	for(int test=1; test<=8; test++)
	{
		//set the operator to AND for tests 1-4, and OR for tests 5-8
		if(test<=4)
			compoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
		else
			compoundExpr->setOperator(Picto::CompoundExpressionOperator::or);

		//deal with the different inversions
		if(test==1 || test ==5)
		{
			compoundExpr->setLHSPredicateExp(predExprA,false);
			compoundExpr->setRHSPredicateExp(predExprB,false);
		}
		else if(test==2 || test ==6)
		{
			compoundExpr->setLHSPredicateExp(predExprA,true);
			compoundExpr->setRHSPredicateExp(predExprB,false);
		}
		else if(test==3 || test ==7)
		{
			compoundExpr->setLHSPredicateExp(predExprA,false);
			compoundExpr->setRHSPredicateExp(predExprB,true);
		}
		else if(test==4 || test ==8)
		{
			compoundExpr->setLHSPredicateExp(predExprA,true);
			compoundExpr->setRHSPredicateExp(predExprB,true);
		}

		//check the string outputs
		if(test == 1)
			QCOMPARE(compoundExpr->toString(true,false),QString("( A Left == 42 ) & ( B Left < 42 )"));
		if(test == 2)
			QCOMPARE(compoundExpr->toString(true,false),QString("!( A Left == 42 ) & ( B Left < 42 )"));
		if(test == 3)
			QCOMPARE(compoundExpr->toString(true,false),QString("( A Left == 42 ) & !( B Left < 42 )"));
		if(test == 4)
			QCOMPARE(compoundExpr->toString(true,false),QString("!( A Left == 42 ) & !( B Left < 42 )"));
		if(test == 5)
			QCOMPARE(compoundExpr->toString(false,true),QString("( 42 == A Right ) | ( 42 < B Right )"));
		if(test == 6)
			QCOMPARE(compoundExpr->toString(false,true),QString("!( 42 == A Right ) | ( 42 < B Right )"));
		if(test == 7)
			QCOMPARE(compoundExpr->toString(false,true),QString("( 42 == A Right ) | !( 42 < B Right )"));
		if(test == 8)
			QCOMPARE(compoundExpr->toString(false,true),QString("!( 42 == A Right ) | !( 42 < B Right )"));

		//output the files...
		QString filename = QString("CompoundExpressionCase1Test%1.bmp").arg(test);

		testImage = compoundExpr->toQImage(true, false);
		testImage.save(filename,"BMP");

	}

	//We aren't cleaning up, because we're going to reuse this compound expression
	
	/////////////////////
	//  Case 2        
	//
	//	This case tests the outpout functionality of a compound expression made
	//	up of two predicate expressions.  A single expression is used (expA & expB),
	//	but all 4 possible choices for outputting values vs names are used.

	//We can recyle the compound expression from case 1
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::and);
	compoundExpr->setLHSPredicateExp(predExprA,false);
	compoundExpr->setRHSPredicateExp(predExprB,false);

	//Test the text output
	//Names only
	QCOMPARE(compoundExpr->toString(true,true),QString("( A Left == A Right ) & ( B Left < B Right )"));

	//Values only
	QCOMPARE(compoundExpr->toString(false,false),QString("( 42 == 42 ) & ( 42 < 42 )"));

	//Names on left, values on right
	QCOMPARE(compoundExpr->toString(true,false),QString("( A Left == 42 ) & ( B Left < 42 )"));

	//Values on left, names on right
	QCOMPARE(compoundExpr->toString(false,true),QString("( 42 == A Right ) & ( 42 < B Right )"));


	//Output the image files
	testImage = compoundExpr->toQImage(true, true);
	testImage.save("CompoundExpressionCase2Test1.bmp","BMP");

	testImage = compoundExpr->toQImage(false, false);
	testImage.save("CompoundExpressionCase2Test2.bmp","BMP");

	testImage = compoundExpr->toQImage(true, false);
	testImage.save("CompoundExpressionCase2Test3.bmp","BMP");

	testImage = compoundExpr->toQImage(false, true);
	testImage.save("CompoundExpressionCase2Test4.bmp","BMP");


	/////////////////////
	//  Case 3        
	//
	//	This case tests the output functionality of this expression:
	//		(expA & expB) | expC
	//We test the output with only names and only values

	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprA->setLHS("A Left");
	predExprA->setRHSParam("A Right");
	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than"));
	predExprB->setLHS("B Left");
	predExprB->setRHSParam("B Right");
	predExprC = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Equal"));
	predExprC->setLHS("C Left");
	predExprC->setRHSParam("C Right");

	compoundExprA->setLHSPredicateExp(predExprA);
	compoundExprA->setRHSPredicateExp(predExprB);
	compoundExprA->setOperator(Picto::CompoundExpressionOperator::and);

	compoundExpr->setLHSCompoundExp(compoundExprA);
	compoundExpr->setRHSPredicateExp(predExprC);
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::or);

	compoundExpr->addScriptables(&paramContainer);

	//Test the text output
	//Names only
	QCOMPARE(compoundExpr->toString(true,true),QString("( ( A Left > A Right ) & ( B Left < B Right ) ) | ( C Left == C Right )"));

	//Values only
	QCOMPARE(compoundExpr->toString(false,false),QString("( ( 42 > 42 ) & ( 42 < 42 ) ) | ( 42 == 42 )"));

	//Output the image files
	testImage = compoundExpr->toQImage(true, true);
	testImage.save("CompoundExpressionCase3Test1.bmp","BMP");

	testImage = compoundExpr->toQImage(false, false);
	testImage.save("CompoundExpressionCase3Test2.bmp","BMP");


	/////////////////////
	//  Case 4        
	//
	//	This case tests the output functionality of this expression:
	//		(expA & expB) | (expC & expD)
	//We test the output with only names and only values

	predExprA = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprA->setLHS("A Left");
	predExprA->setRHSParam("A Right");
	predExprB = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Less than"));
	predExprB->setLHS("B Left");
	predExprB->setRHSParam("B Right");
	predExprC = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Equal"));
	predExprC->setLHS("C Left");
	predExprC->setRHSParam("C Right");
	predExprD = QSharedPointer<Picto::PredicateExpression>(new Picto::PredicateExpression("Greater than"));
	predExprD->setLHS("D Left");
	predExprD->setRHSParam("D Right");

	compoundExprA->setLHSPredicateExp(predExprA);
	compoundExprA->setRHSPredicateExp(predExprB);
	compoundExprA->setOperator(Picto::CompoundExpressionOperator::and);

	compoundExprB->setLHSPredicateExp(predExprC);
	compoundExprB->setRHSPredicateExp(predExprD);
	compoundExprB->setOperator(Picto::CompoundExpressionOperator::and);

	compoundExpr->setLHSCompoundExp(compoundExprA);
	compoundExpr->setRHSCompoundExp(compoundExprB);
	compoundExpr->setOperator(Picto::CompoundExpressionOperator::or);

	compoundExpr->addScriptables(&paramContainer);

	//Test the text output
	//Names only
	QCOMPARE(compoundExpr->toString(true,true),QString("( ( A Left > A Right ) & ( B Left < B Right ) ) | ( ( C Left == C Right ) & ( D Left > D Right ) )"));

	//Values only
	QCOMPARE(compoundExpr->toString(false,false),QString("( ( 42 > 42 ) & ( 42 < 42 ) ) | ( ( 42 == 42 ) & ( 42 > 42 ) )"));

	//Output image files
	testImage = compoundExpr->toQImage(true, true);
	testImage.save("CompoundExpressionCase4Test1.bmp","BMP");

	testImage = compoundExpr->toQImage(false, false);
	testImage.save("CompoundExpressionCase4Test2.bmp","BMP");


	QWARN("Don't forget to check the image files to confirm output");

}
