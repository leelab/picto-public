/*#ifndef _ENGINE_TEST_H
#define _ENGINE_TEST_H

#include <QObject>

#include "../common/engine/PictoEngine.h"
#include "../common/random/MersenneTwister.h"

class EngineTest : public QObject
{
	Q_OBJECT

public:
	EngineTest();
	~EngineTest() {};

	void exec();
	void TestLayerRendering();
	void TestStateMachine();

private:
	Picto::Engine::PictoEngine engine_;
	Picto::MTRand randGen_;




};

#endif*/
