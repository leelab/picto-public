/*! \brief Contains unit tests for Predicate objects
 *
 * 	This tests the objects: Predicate, PredicateExpression, and CompoundExpression.
 *	Although most of the tests are automated, the graphics tests can't be automated.
 *	Instead, to test the toQImage() functionality, a jpg image is generated, and
 *	the user is prompted to confirm its correctness.
 */

#ifndef _TEST_DATA_STORE_H
#define _TEST_DATA_STORE_H

#include <QtTest/QtTest>

#include "../../common/globals.h"
#include "../random/MersenneTwister.h"
#include "../stimuli/VisualElement.h"
#include "../stimuli/VisualElementFactory.h"


class TestDataStore: public QObject
{
    Q_OBJECT

public:
	TestDataStore();

private slots:
	void TestVisualElementDataStore_data();
	void TestVisualElementDataStore();

private:
	Picto::MTRand randGen_;
	Picto::VisualElementFactory visualElementFactory_;

	void testSingleVisualElementDataStore(Picto::VisualElement *original);
};


#endif