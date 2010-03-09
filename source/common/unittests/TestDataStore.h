/*! \brief Contains unit tests for DataStore objects
 *
 * 	This tests the objects that inherit from DataStore.  Most objects are tested
 *	by creating a random instance of the object, serializing it, deserializing
 *	the generated XML fragment into a new object, and then serializing that object
 *	and comparing the resulting XML fragmenet to the original.  This isn't a totally
 *	comprehensive test, since it's possible that the copy object generates identical 
 *	XML without being an identical copy, but this seems like the easiest way to test.
 *	
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