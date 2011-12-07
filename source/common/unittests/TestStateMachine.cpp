#include "TestStateMachine.h"
#include "../memleakdetect.h"

TestStateMachine::TestStateMachine()
{
	randGen_.seed();

}

/*! \brief Tests the rendering to screen performed by the Layer object 
void TestStateMachine::TestRendering()
{
	//Create a layer with a couple of VisualElements
	Picto::Layer layer1;

	QSharedPointer<Picto::BoxGraphic> boxGraphic1(new Picto::BoxGraphic(QPoint(30,30),QRect(0,0,50,100),QColor(255,255,0,255)));
	layer1.addVisualElement(boxGraphic1);

	layer1.draw();

	QTest::qWait(5000);

}*/
