
#ifndef _TEST_STATE_MACHINE_H
#define _TEST_STATE_MACHINE_H

#include <QtTest/QtTest>

#include "../random/MersenneTwister.h"
#include "../globals.h"
#include "../engine/PictoEngine.h"

/*! \brief Contains tests for StateMachine objects
 *
 * 	These tests aren't particularly automated (I can't Q_COMPARE the screen
 *	output to what I think it should be), but are more of a test of system functionality.
 *
 */
class TestStateMachine: public QObject
{
    Q_OBJECT

public:
	TestStateMachine();

private:
	Picto::MTRand randGen_;

private slots:
	//void TestRendering();
};


#endif