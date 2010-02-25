/*! \brief Contains unit tests for Predicate objects
 *
 * 	This tests the objects: Predicate, PredicateExpression, and CompoundExpression.
 *	Although most of the tests are automated, the graphics tests can't be automated.
 *	Instead, to test the toQImage() functionality, a jpg image is generated, and
 *	the user is prompted to confirm its correctness.
 */

#ifndef _TEST_PARAMETERS_H
#define _TEST_PARAMETERS_H

#include <QtTest/QtTest>

#include "../../common/globals.h"
#include "../random/MersenneTwister.h"


class TestParameters: public QObject
{
    Q_OBJECT

public:
	TestParameters();

private slots:
	void TestBooleanParameter();
	void TestRangeParameter();
	void TestChoiceParameter();
	void TestNumericParameter();

private:
	Picto::MTRand randGen_;
};


#endif