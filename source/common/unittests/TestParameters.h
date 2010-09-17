#ifndef _TEST_PARAMETERS_H
#define _TEST_PARAMETERS_H

#include <QtTest/QtTest>

#include "../../common/globals.h"
#include "../random/MersenneTwister.h"


/*! \brief Contains unit tests for Parameter objects
 *
 * 	This tests the boolean, range, choice, and numeric parameters.  The
 *	tests are pretty much self-explanatory.
 */

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