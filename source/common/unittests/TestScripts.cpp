#include "TestSCripts.h"
#include "../parameter/ParameterContainer.h"
#include "../parameter/NumericParameter.h"
#include "../parameter/BooleanParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/RangeParameter.h"


TestScripts::TestScripts()
{
	randGen_.seed();
}



void TestScripts::TestParameterBinding()
{
	//Setup
	//-----
	QString script;
	Picto::ParameterContainer parameters;

	//Load up the parameter container
	QSharedPointer<Picto::NumericParameter> param1(new Picto::NumericParameter);
	param1->setName("TrialNumber");
	param1->setValue(1);
	parameters.addParameter(param1);

	QSharedPointer<Picto::BooleanParameter> param2(new Picto::BooleanParameter);
	param2->setName("passed");
	param2->setValue(false);
	parameters.addParameter(param2);


	QSharedPointer<Picto::ChoiceParameter> param3(new Picto::ChoiceParameter);
	param3->setName("color");

	param3->addChoice("Red",QColor(255,0,0));
	param3->addChoice("Green",QColor(0,255,0));
	param3->addChoice("Blue",QColor(255,0,0));
	param3->setValue("Green");
	parameters.addParameter(param3);

	QSharedPointer<Picto::RangeParameter> param4(new Picto::RangeParameter);
	param4->setName("NumberOfTargets");
	param4->setMin(2);
	param4->setMax(10);
	param4->setIncrement(2);
	param4->setValue(4);
	parameters.addParameter(param4);

	//Bind everything
	parameters.addAsScriptProperties(qsEngine_);

	//Test the numeric parameter
	//--------------------------
	QSharedPointer<Picto::Parameter> testParam;
	testParam = parameters.getParameter("TrialNumber");
	QCOMPARE(testParam->getValue().toInt(),1);

	script = "TrialNumber.setValue(TrialNumber.getValue() + 5);\n";
	qsEngine_.evaluate(script);
				
	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().toInt(),6);

	//Test the boolean parameter
	//--------------------------
	testParam = parameters.getParameter("passed");
	QCOMPARE(testParam->getValue().toBool(),false);

	script = "passed.setValue(true);\n";
	qsEngine_.evaluate(script);

	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().toBool(),true);

	//Test the choice parameter
	//-------------------------
	testParam = parameters.getParameter("color");
	QCOMPARE(testParam->getValue().toString(),QString("#00ff00"));

	script = "color.setValue(\"Red\");\n";
	qsEngine_.evaluate(script);

	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().value<QColor>(),QColor(255,0,0));

	//Test the range parameter
	//------------------------
	testParam = parameters.getParameter("NumberOfTargets");
	QCOMPARE(testParam->getValue().toInt(),4);

	script = "NumberOfTargets.setValue(8);\n";
	qsEngine_.evaluate(script);
				
	QCOMPARE(qsEngine_.hasUncaughtException(), false);
	QCOMPARE(testParam->getValue().toInt(),8);

}