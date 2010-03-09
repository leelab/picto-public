/*! \brief Contains unit tests for scripting
 *
 * 	Since I've never dealt with scripting in a Qt environment before, these 
 *	tests serve two purposes:
 *		1. As a testbed to see if I can get scripting working
 *		2. To test that the scripting is working correctly
 */

#ifndef _TEST_SCRIPTS_H
#define _TEST_SCRIPTS_H

#include <QtTest/QtTest>
#include <QScriptEngine>
#include <QScriptValue>

#include "../../common/globals.h"
#include "../random/MersenneTwister.h"
#include "../parameter/Parameter.h"


class TestScripts: public QObject
{
    Q_OBJECT

public:
	TestScripts();


private slots:
	void TestParameterBinding();
private:
	Picto::MTRand randGen_;
	QScriptEngine qsEngine_;

};


#endif