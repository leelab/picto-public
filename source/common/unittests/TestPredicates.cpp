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



TestPredicates::TestPredicates()
{
	randGen.seed();
}


/*!	\brief Tests that single predicates are evaluating correctly
 *
 *	This test runs through all of the existing predicates and tests
 *	their "evaluate" function.  Since there are an infinite number of
 *	test cases, we randomly select pairs of values and test them, making
 *	sure to exercise all possible relationships.
 */
void TestPredicates::TestPredicateEvaluation()
{
	Picto::Predicate *pred;
	double A, B;
	int numTests = 10;
	double maxValue = 5000;
	double minValue = -5000;

	////////////////////////////
	// PredicateGreaterThan
	//
	pred = new Picto::PredicateGreaterThan();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A>B));
		QCOMPARE(pred->evaluate(B,A), (B>A));
		QCOMPARE(pred->evaluate(A,A), (A>A));
		QCOMPARE(pred->evaluate(B,B), (B>B));
	}

	delete pred;

	////////////////////////////
	// PredicateGreaterThanEq
	//
	pred = new Picto::PredicateGreaterThanEq();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A>=B));
		QCOMPARE(pred->evaluate(B,A), (B>=A));
		QCOMPARE(pred->evaluate(A,A), (A>=A));
		QCOMPARE(pred->evaluate(B,B), (B>=B));
	}

	delete pred;

	////////////////////////////
	// PredicateLessThan
	//
	pred = new Picto::PredicateLessThan();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A<B));
		QCOMPARE(pred->evaluate(B,A), (B<A));
		QCOMPARE(pred->evaluate(A,A), (A<A));
		QCOMPARE(pred->evaluate(B,B), (B<B));
	}

	delete pred;

	////////////////////////////
	// PredicateLessThanEq
	//
	pred = new Picto::PredicateLessThanEq();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A<=B));
		QCOMPARE(pred->evaluate(B,A), (B<=A));
		QCOMPARE(pred->evaluate(A,A), (A<=A));
		QCOMPARE(pred->evaluate(B,B), (B<=B));
	}

	delete pred;

	////////////////////////////
	// PredicateEqual
	//
	pred = new Picto::PredicateEqual();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A==B));
		QCOMPARE(pred->evaluate(B,A), (B==A));
		QCOMPARE(pred->evaluate(A,A), (A==A));
		QCOMPARE(pred->evaluate(B,B), (B==B));
	}

	delete pred;

	////////////////////////////
	// PredicateNotEqual
	//
	pred = new Picto::PredicateNotEqual();

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four possible combinations
		QCOMPARE(pred->evaluate(A,B), (A!=B));
		QCOMPARE(pred->evaluate(B,A), (B!=A));
		QCOMPARE(pred->evaluate(A,A), (A!=A));
		QCOMPARE(pred->evaluate(B,B), (B!=B));
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

/*!	Tests the constructor, destructor,getters, setters, and isValid of the PredicateExpression Object
 *
 *	This is a general test of the PredicateExpression object.  It tests the following
 *	functionality:
 *	- All three construstors
 *	- All getter and setter functions
 *	- isValid function
 *	The testing is thorough, but by no means comprehensive.
 */
void TestPredicates::TestPredicateExpressionGeneral()
{
	Picto::PredicateExpression *predExp;
	Picto::Predicate *pred;
	Picto::Predicate *testPred;

	//Subtest 1:
	//------------------------------------------
	//	Construct an empty expression
	//	Confirm expression is invalid
	//	Set, then get the predicate
	//	Confirm expression is still invalid
	//	Set, then get the LHS (name & value)
	//	Confirm expression is still invalid
	//	Set, then get the RHS (name & value)
	//	Confirm expression is valid
	predExp = new Picto::PredicateExpression();
	pred = new Picto::PredicateGreaterThan();
	
	QCOMPARE(predExp->isValid(), false);

	predExp->setPredicate(pred);
	testPred = predExp->getPredicate();
	QCOMPARE(testPred->toString(),QString(">"));

	QCOMPARE(predExp->isValid(), false);

	predExp->setLHS(31,"left side");
	QCOMPARE(predExp->getLHSvalue(),31.0);
	QCOMPARE(predExp->getLHSname(),QString("left side"));

	QCOMPARE(predExp->isValid(), false);

	predExp->setRHS(32,"right side");
	QCOMPARE(predExp->getRHSvalue(),32.0);
	QCOMPARE(predExp->getRHSname(),QString("right side"));

	QCOMPARE(predExp->isValid(), true);


	delete predExp;
	delete pred;


	//Subtest 2:
	//------------------------------------------
	//	Construct an expression with a predicate
	//	Get the predicate and confirm it is correct
	//	Confirm expression is invalid
	//	Set LHS and RHS
	//	Confirm expression is valid
	pred = new Picto::PredicateGreaterThan();
	predExp = new Picto::PredicateExpression(pred);

	testPred = predExp->getPredicate();
	QCOMPARE(testPred->toString(), QString(">"));

	QCOMPARE(predExp->isValid(),false);

	predExp->setLHS(31,"left side");
	predExp->setRHS(32,"right side");

	QCOMPARE(predExp->isValid(),true);

	delete pred;
	delete predExp;

	//Subtest 3:
	//------------------------------------------
	//	Construct an expression with a predicate, LHS and RHS (names and values)
	//	Get the predicate and confirm it is correct
	//	Get the LHS value and name and confim they are correct
	//	Get the RHS value and name and confim they are correct
	//	Confirm expression is valid
	pred = new Picto::PredicateGreaterThan();
	predExp = new Picto::PredicateExpression(pred,19,20,"left side", "right side");

	testPred = predExp->getPredicate();
	QCOMPARE(testPred->toString(), QString(">"));

	QCOMPARE(predExp->getLHSvalue(),19.0);
	QCOMPARE(predExp->getLHSname(),QString("left side"));

	QCOMPARE(predExp->getRHSvalue(),20.0);
	QCOMPARE(predExp->getRHSname(),QString("right side"));

	QCOMPARE(predExp->isValid(), true);
	delete pred;
	delete predExp;
}

/*!	\brief Tests the evaluation of predicate expressions 
 *
 *	Throughly testing the evaluation of predicate expressions is actually
 *	really simple.  Since the predicate evaluation is already tested, I 
 *	can just test pick one of the predicates and use it.
 */
void TestPredicates::TestPredicateExpressionEvaluation()
{
	Picto::PredicateExpression *predExp;
	Picto::Predicate *pred;

	pred = new Picto::PredicateGreaterThan();
	predExp = new Picto::PredicateExpression(pred);

	double A, B;
	int numTests = 10;
	double maxValue = 5000;
	double minValue = -5000;

	for(int i=0; i<numTests; i++)
	{
		//generate random values
		A = randGen.randDblExc(maxValue-minValue) + minValue;
		B = randGen.randDblExc(maxValue-minValue) + minValue;

		//test all four combinations (this also tests the set function)
		predExp->setLHS(A);
		predExp->setRHS(B);
		QCOMPARE(predExp->evaluate(),(A>B));

		predExp->setLHS(B);
		predExp->setRHS(A);
		QCOMPARE(predExp->evaluate(),(B>A));

		predExp->setLHS(A);
		predExp->setRHS(A);
		QCOMPARE(predExp->evaluate(),(A>A));

		predExp->setLHS(B);
		predExp->setRHS(B);
		QCOMPARE(predExp->evaluate(),(B>B));
	}

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
	Picto::PredicateExpression *predExp;
	Picto::Predicate *pred;

	////////////////////////////////
	//Test the text output
	//
	pred = new Picto::PredicateGreaterThan();
	predExp = new Picto::PredicateExpression(pred,25,-3.1415,"foo", "bar");

	QCOMPARE(predExp->toString(false,false),QString("25 > -3.1415"));
	QCOMPARE(predExp->toString(true,false),QString("foo > -3.1415"));
	QCOMPARE(predExp->toString(false,true),QString("25 > bar"));
	QCOMPARE(predExp->toString(true,true),QString("foo > bar"));

	delete predExp;

	//what happens if there isn't a name for the side?
	predExp = new Picto::PredicateExpression(pred,25,-3.1415);
	QCOMPARE(predExp->toString(true,true),QString("25 > -3.1415"));

	delete predExp;


	////////////////////////////////
	//Test the image output
	//
	predExp = new Picto::PredicateExpression(pred,25,-3.1415,"foo", "bar");
	QImage testImage;

	testImage = predExp->toQImage(false, false);
	testImage.save("PredicateExpression1.bmp","BMP");

	testImage = predExp->toQImage(true, false);
	testImage.save("PredicateExpression2.bmp","BMP");

	testImage = predExp->toQImage(false, true);
	testImage.save("PredicateExpression3.bmp","BMP");

	testImage = predExp->toQImage(true, true);
	testImage.save("PredicateExpression4.bmp","BMP");

	delete predExp;

	//what happens if there isn't a name for the side?
	predExp = new Picto::PredicateExpression(pred,25,-3.1415);
	testImage = predExp->toQImage(true, true);
	testImage.save("PredicateExpression5.bmp","BMP");

	delete predExp;
	
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
	Picto::CompoundExpression *compoundExpr, *compoundExprA, *compoundExprB;
	Picto::PredicateExpression *predExprA, *predExprB, *predExprC, *predExprD;
	Picto::Predicate *pred;

	double A_L, A_R, B_L, B_R, C_L, C_R, D_L, D_R;
	int numTests = 10;
	double maxValue = 5000;
	double minValue = -5000;

	/////////////////////
	//  Case 1        
	//
	//	This case tests the basic functionality of a compound expression made
	//	up of two predicate expressions.  The full range of possibilities are
	//	tested (and, or, invert either or both sides of the expression).  Random 
	//	values are used to really stretch the object.

	pred = new Picto::PredicateGreaterThan();
	predExprA = new Picto::PredicateExpression(pred,1,1);
	predExprB = new Picto::PredicateExpression(pred,1,1);
	compoundExpr = new Picto::CompoundExpression(predExprA, predExprB, 
							Picto::CompoundExpressionOperator::and,
							false,false);


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
			A_L = randGen.randDblExc(maxValue-minValue) + minValue;
			A_R = randGen.randDblExc(maxValue-minValue) + minValue;
			B_L = randGen.randDblExc(maxValue-minValue) + minValue;
			B_R = randGen.randDblExc(maxValue-minValue) + minValue;

			predExprA->setLHS(A_L);
			predExprA->setRHS(A_R);
			predExprB->setLHS(B_L);
			predExprB->setRHS(B_R);

			if(test==1)
				QCOMPARE(compoundExpr->evaluate(), (A_L>A_R) && (B_L > B_R));
			if(test==2)
				QCOMPARE(compoundExpr->evaluate(), !(A_L>A_R) && (B_L > B_R));
			if(test==3)
				QCOMPARE(compoundExpr->evaluate(), (A_L>A_R) && !(B_L > B_R));
			if(test==4)
				QCOMPARE(compoundExpr->evaluate(), !(A_L>A_R) && !(B_L > B_R));
			if(test==5)
				QCOMPARE(compoundExpr->evaluate(), (A_L>A_R) || (B_L > B_R));
			if(test==6)
				QCOMPARE(compoundExpr->evaluate(), !(A_L>A_R) || (B_L > B_R));
			if(test==7)
				QCOMPARE(compoundExpr->evaluate(), (A_L>A_R) || !(B_L > B_R));
			if(test==8)
				QCOMPARE(compoundExpr->evaluate(), !(A_L>A_R) || !(B_L > B_R));
		}
	}

	//clean up
	delete pred;
	delete predExprA;
	delete predExprB;
	delete compoundExpr;

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

	pred = new Picto::PredicateGreaterThan();
	predExprA = new Picto::PredicateExpression(pred,1,1);
	predExprB = new Picto::PredicateExpression(pred,1,1);
	predExprC = new Picto::PredicateExpression(pred,1,1);
	compoundExprA = new Picto::CompoundExpression(predExprA, predExprB, 
							Picto::CompoundExpressionOperator::and,
							false,false);
	compoundExpr = new Picto::CompoundExpression(compoundExprA, predExprC, 
							Picto::CompoundExpressionOperator::or,
							false,false);

	for(int i=0; i<numTests; i++)
	{
		A_L = randGen.randDblExc(maxValue-minValue) + minValue;
		A_R = randGen.randDblExc(maxValue-minValue) + minValue;
		B_L = randGen.randDblExc(maxValue-minValue) + minValue;
		B_R = randGen.randDblExc(maxValue-minValue) + minValue;
		C_L = randGen.randDblExc(maxValue-minValue) + minValue;
		C_R = randGen.randDblExc(maxValue-minValue) + minValue;

		predExprA->setLHS(A_L);
		predExprA->setRHS(A_R);
		predExprB->setLHS(B_L);
		predExprB->setRHS(B_R);
		predExprC->setLHS(C_L);
		predExprC->setRHS(C_R);

		QCOMPARE(compoundExpr->evaluate(), ((A_L > A_R) && (B_L > B_R)) || (C_L > C_R));
	}

	//cleanup
	delete pred;
	delete predExprA;
	delete predExprB;
	delete predExprC;
	delete compoundExprA;
	delete compoundExpr;

	/////////////////////
	//  Case 3        
	//
	//	This case tests the basic functionality of a compound expression made
	//	up of a predicate expressions and a coupound expression.  The expression 
	//	being used is:
	//		(expA AND expB) OR (expC AND expD)

	pred = new Picto::PredicateGreaterThan();
	predExprA = new Picto::PredicateExpression(pred,1,1);
	predExprB = new Picto::PredicateExpression(pred,1,1);
	predExprC = new Picto::PredicateExpression(pred,1,1);
	predExprD = new Picto::PredicateExpression(pred,1,1);
	compoundExprA = new Picto::CompoundExpression(predExprA, predExprB, 
							Picto::CompoundExpressionOperator::and,
							false,false);
	compoundExprB = new Picto::CompoundExpression(predExprC, predExprD, 
							Picto::CompoundExpressionOperator::and,
							false,false);
	compoundExpr = new Picto::CompoundExpression(compoundExprA, compoundExprB, 
							Picto::CompoundExpressionOperator::or,
							false,false);

	for(int i=0; i<numTests; i++)
	{
		A_L = randGen.randDblExc(maxValue-minValue) + minValue;
		A_R = randGen.randDblExc(maxValue-minValue) + minValue;
		B_L = randGen.randDblExc(maxValue-minValue) + minValue;
		B_R = randGen.randDblExc(maxValue-minValue) + minValue;
		C_L = randGen.randDblExc(maxValue-minValue) + minValue;
		C_R = randGen.randDblExc(maxValue-minValue) + minValue;
		D_L = randGen.randDblExc(maxValue-minValue) + minValue;
		D_R = randGen.randDblExc(maxValue-minValue) + minValue;

		predExprA->setLHS(A_L);
		predExprA->setRHS(A_R);
		predExprB->setLHS(B_L);
		predExprB->setRHS(B_R);
		predExprC->setLHS(C_L);
		predExprC->setRHS(C_R);
		predExprD->setLHS(D_L);
		predExprD->setRHS(D_R);

		QCOMPARE(compoundExpr->evaluate(), ((A_L > A_R) && (B_L > B_R)) || ((C_L > C_R) && (D_L > D_R)));
	}

	//cleanup
	delete pred;
	delete predExprA;
	delete predExprB;
	delete predExprC;
	delete predExprD;
	delete compoundExprA;
	delete compoundExprB;
	delete compoundExpr;

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
 */
void TestPredicates::TestCompoundExpressionOutputs()
{
	Picto::CompoundExpression *compoundExpr, *compoundExprA, *compoundExprB;
	Picto::PredicateExpression *predExprA, *predExprB, *predExprC, *predExprD;
	Picto::Predicate *pred;

	QImage testImage;

	/////////////////////
	//  Case 1        
	//
	//	This case tests the oupout functionality of a compound expression made
	//	up of two predicate expressions.  The full range of possibilities are
	//	tested (and, or, invert either or both sides of the expression) with the
	//	expression, but the display options are limited to values only

	pred = new Picto::PredicateGreaterThan();
	predExprA = new Picto::PredicateExpression(pred,1,2, "ALeft", "ARight");
	predExprB = new Picto::PredicateExpression(pred,3.14,-2.86, "BLeft", "BRight");
	compoundExpr = new Picto::CompoundExpression(predExprA, predExprB, 
							Picto::CompoundExpressionOperator::and,
							false,false);


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
			QCOMPARE(compoundExpr->toString(false,false),QString("( 1 > 2 ) & ( 3.14 > -2.86 )"));
		if(test == 2)
			QCOMPARE(compoundExpr->toString(false,false),QString("!( 1 > 2 ) & ( 3.14 > -2.86 )"));
		if(test == 3)
			QCOMPARE(compoundExpr->toString(false,false),QString("( 1 > 2 ) & !( 3.14 > -2.86 )"));
		if(test == 4)
			QCOMPARE(compoundExpr->toString(false,false),QString("!( 1 > 2 ) & !( 3.14 > -2.86 )"));
		if(test == 5)
			QCOMPARE(compoundExpr->toString(false,false),QString("( 1 > 2 ) | ( 3.14 > -2.86 )"));
		if(test == 6)
			QCOMPARE(compoundExpr->toString(false,false),QString("!( 1 > 2 ) | ( 3.14 > -2.86 )"));
		if(test == 7)
			QCOMPARE(compoundExpr->toString(false,false),QString("( 1 > 2 ) | !( 3.14 > -2.86 )"));
		if(test == 8)
			QCOMPARE(compoundExpr->toString(false,false),QString("!( 1 > 2 ) | !( 3.14 > -2.86 )"));

		//output the files...
		QString filename = QString("CompoundExpressionCase1Test%1.bmp").arg(test);

		testImage = compoundExpr->toQImage(false, false);
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
	QCOMPARE(compoundExpr->toString(true,true),QString("( ALeft > ARight ) & ( BLeft > BRight )"));

	//Values only
	QCOMPARE(compoundExpr->toString(false,false),QString("( 1 > 2 ) & ( 3.14 > -2.86 )"));

	//Names on left, values on right
	QCOMPARE(compoundExpr->toString(true,false),QString("( ALeft > 2 ) & ( BLeft > -2.86 )"));

	//Values on left, names on right
	QCOMPARE(compoundExpr->toString(false,true),QString("( 1 > ARight ) & ( 3.14 > BRight )"));


	//Output the image files
	testImage = compoundExpr->toQImage(true, true);
	testImage.save("CompoundExpressionCase2Test1.bmp","BMP");

	testImage = compoundExpr->toQImage(false, false);
	testImage.save("CompoundExpressionCase2Test2.bmp","BMP");

	testImage = compoundExpr->toQImage(true, false);
	testImage.save("CompoundExpressionCase2Test3.bmp","BMP");

	testImage = compoundExpr->toQImage(false, true);
	testImage.save("CompoundExpressionCase2Test4.bmp","BMP");


	//clean up
	delete pred;
	delete predExprA;
	delete predExprB;
	delete compoundExpr;



	/////////////////////
	//  Case 3        
	//
	//	This case tests the output functionality of this expression:
	//		(expA & expB) | expC
	//We test the output with only names and only values

	pred = new Picto::PredicateGreaterThan();
	predExprA = new Picto::PredicateExpression(pred,1,2, "ALeft", "ARight");
	predExprB = new Picto::PredicateExpression(pred,3,4, "BLeft", "BRight");
	predExprC = new Picto::PredicateExpression(pred,5,6, "CLeft", "CRight");
	compoundExprA = new Picto::CompoundExpression(predExprA, predExprB, 
							Picto::CompoundExpressionOperator::and,
							false,false);
	compoundExpr = new Picto::CompoundExpression(compoundExprA, predExprC, 
							Picto::CompoundExpressionOperator::or,
							false,false);

	//Test the text output
	//Names only
	QCOMPARE(compoundExpr->toString(true,true),QString("( ( ALeft > ARight ) & ( BLeft > BRight ) ) | ( CLeft > CRight )"));

	//Values only
	QCOMPARE(compoundExpr->toString(false,false),QString("( ( 1 > 2 ) & ( 3 > 4 ) ) | ( 5 > 6 )"));

	//Output the image files
	testImage = compoundExpr->toQImage(true, true);
	testImage.save("CompoundExpressionCase3Test1.bmp","BMP");

	testImage = compoundExpr->toQImage(false, false);
	testImage.save("CompoundExpressionCase3Test2.bmp","BMP");


	//clean up
	delete pred;
	delete predExprA;
	delete predExprB;
	delete predExprC;
	delete compoundExprA;
	delete compoundExpr;


	/////////////////////
	//  Case 4        
	//
	//	This case tests the output functionality of this expression:
	//		(expA & expB) | (expC & expD)
	//We test the output with only names and only values

	pred = new Picto::PredicateGreaterThan();
	predExprA = new Picto::PredicateExpression(pred,1,2, "ALeft", "ARight");
	predExprB = new Picto::PredicateExpression(pred,3,4, "BLeft", "BRight");
	predExprC = new Picto::PredicateExpression(pred,5,6, "CLeft", "CRight");
	predExprD = new Picto::PredicateExpression(pred,7,8, "DLeft", "DRight");
	compoundExprA = new Picto::CompoundExpression(predExprA, predExprB, 
							Picto::CompoundExpressionOperator::and,
							false,false);
	compoundExprB = new Picto::CompoundExpression(predExprC, predExprD, 
							Picto::CompoundExpressionOperator::and,
							false,false);
	compoundExpr = new Picto::CompoundExpression(compoundExprA, compoundExprB, 
							Picto::CompoundExpressionOperator::or,
							false,false);


	//Test the text output
	//Names only
	QCOMPARE(compoundExpr->toString(true,true),QString("( ( ALeft > ARight ) & ( BLeft > BRight ) ) | ( ( CLeft > CRight ) & ( DLeft > DRight ) )"));

	//Values only
	QCOMPARE(compoundExpr->toString(false,false),QString("( ( 1 > 2 ) & ( 3 > 4 ) ) | ( ( 5 > 6 ) & ( 7 > 8 ) )"));

	//Output image files
	testImage = compoundExpr->toQImage(true, true);
	testImage.save("CompoundExpressionCase4Test1.bmp","BMP");

	testImage = compoundExpr->toQImage(false, false);
	testImage.save("CompoundExpressionCase4Test2.bmp","BMP");


	//clean up
	delete pred;
	delete predExprA;
	delete predExprB;
	delete predExprC;
	delete predExprD;
	delete compoundExprA;
	delete compoundExprB;
	delete compoundExpr;

	QWARN("Don't forget to check the image files to confirm output");

}
