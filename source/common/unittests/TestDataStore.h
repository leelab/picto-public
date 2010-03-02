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
#include "../parameter/Parameter.h"
#include "../parameter/ParameterFactory.h"


class TestDataStore: public QObject
{
    Q_OBJECT

public:
	TestDataStore();

private slots:
	void TestVisualElement_data();
	void TestVisualElement();
	void TestParameter_data();
	void TestParameter();
	void TestParameterContainer();
	void TestPredicateExpression();
	void TestCompoundExpression();
	void TestScene();
	void TestStateMachine();
private:
	Picto::MTRand randGen_;
	Picto::VisualElementFactory visualElementFactory_;

	void TestSimpleDataStore(QSharedPointer<Picto::DataStore> original, QSharedPointer<Picto::DataStore> copy, QString startElementName);

};


#endif