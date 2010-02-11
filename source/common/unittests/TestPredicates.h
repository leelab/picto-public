#include <QtTest/QtTest>

#include "../random/MersenneTwister.h"
#include "../../common/globals.h"


/*! \brief Contains unit tests for Predicate objects
 *
 * 	This tests the objects: Predicate, PredicateExpression, and CompoundExpression.
 *	Although most of the tests are automated, the graphics tests can't be automated.
 *	Instead, to test the toQImage() functionality, a jpg image is generated, and
 *	the user is prompted to confirm its correctness.
 */

class TestPredicates: public QObject
{
    Q_OBJECT

public:
	TestPredicates();

private:
	Picto::MTRand randGen;

private slots:
	void TestPredicateEvaluation();
	void TestPredicateOutputs();

	void TestPredicateExpressionGeneral();
	void TestPredicateExpressionEvaluation();
	void TestPredicateExpressionOutputs();

	void TestCompoundExpressionEvaluation();
	void TestCompoundExpressionOutputs();
};


