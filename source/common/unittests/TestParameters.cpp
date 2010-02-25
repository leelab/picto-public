#include <QFile>
#include <QSharedPointer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "TestParameters.h"
#include "../parameter/BooleanParameter.h"
#include "../parameter/RangeParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/NumericParameter.h"

TestParameters::TestParameters()
{
	randGen_.seed();
}


/*!	\brief Tests boolean parameter
 *	The testing consists of:
 *		1. Creating a new booleanParameter
 *		2. Confirming that the default value is false
 *		3. Setting the value to true/false with setValue, then
 *		   confirming that the value was set with getValue.
 *		4. Creating two boolean parameters and checking that
 *		   the comparison functions all work.
 *		5. Creating two boolean parameters and checking that
 *		   the comparison operators all work.
*/
void TestParameters::TestBooleanParameter()
{
	Picto::BooleanParameter param1;
	Picto::BooleanParameter param2;
	
	//Confirm that the default value is false
	QCOMPARE(param1.getValue().toBool(), false);

	//Set the value to true/false with setValue, then confirming with getValue.
	param1.setValue(QVariant(true));
	QCOMPARE(param1.getValue().toBool(), true);
	param1.setValue(QVariant(false));
	QCOMPARE(param1.getValue().toBool(), false);

	//Check the comparison functions and operators in all possible combinations
	param1.setValue(QVariant(false));
	param2.setValue(QVariant(false));
	QCOMPARE(param1.greaterThan(param2), false);
	QCOMPARE(param1.greaterThanEq(param2), true);
	QCOMPARE(param1.lessThan(param2), false);
	QCOMPARE(param1.lessThanEq(param2), true);
	QCOMPARE(param1.equalTo(param2), true);
	QCOMPARE(param1.notEqualTo(param2), false);
	QCOMPARE(param1 > param2, false);
	QCOMPARE(param1 >= param2, true);
	QCOMPARE(param1 < param2, false);
	QCOMPARE(param1 <= param2, true);
	QCOMPARE(param1 == param2, true);
	QCOMPARE(param1 != param2, false);

	param1.setValue(QVariant(true));
	param2.setValue(QVariant(false));
	QCOMPARE(param1.greaterThan(param2), false);
	QCOMPARE(param1.greaterThanEq(param2), false);
	QCOMPARE(param1.lessThan(param2), false);
	QCOMPARE(param1.lessThanEq(param2), false);
	QCOMPARE(param1.equalTo(param2), false);
	QCOMPARE(param1.notEqualTo(param2), true);
	QCOMPARE(param1 > param2, false);
	QCOMPARE(param1 >= param2, false);
	QCOMPARE(param1 < param2, false);
	QCOMPARE(param1 <= param2, false);
	QCOMPARE(param1 == param2, false);
	QCOMPARE(param1 != param2, true);

	param1.setValue(QVariant(false));
	param2.setValue(QVariant(true));
	QCOMPARE(param1.greaterThan(param2), false);
	QCOMPARE(param1.greaterThanEq(param2), false);
	QCOMPARE(param1.lessThan(param2), false);
	QCOMPARE(param1.lessThanEq(param2), false);
	QCOMPARE(param1.equalTo(param2), false);
	QCOMPARE(param1.notEqualTo(param2), true);
	QCOMPARE(param1 > param2, false);
	QCOMPARE(param1 >= param2, false);
	QCOMPARE(param1 < param2, false);
	QCOMPARE(param1 <= param2, false);
	QCOMPARE(param1 == param2, false);
	QCOMPARE(param1 != param2, true);

	param1.setValue(QVariant(true));
	param2.setValue(QVariant(true));
	QCOMPARE(param1.greaterThan(param2), false);
	QCOMPARE(param1.greaterThanEq(param2), true);
	QCOMPARE(param1.lessThan(param2), false);
	QCOMPARE(param1.lessThanEq(param2), true);
	QCOMPARE(param1.equalTo(param2), true);
	QCOMPARE(param1.notEqualTo(param2), false);
	QCOMPARE(param1 > param2, false);
	QCOMPARE(param1 >= param2, true);
	QCOMPARE(param1 < param2, false);
	QCOMPARE(param1 <= param2, true);
	QCOMPARE(param1 == param2, true);
	QCOMPARE(param1 != param2, false);

}


/*!	\brief Tests range parameter
 *	The testing consists of:
 *		1. Create a new RangeParameter
 *		2. Set min/max/increment/default
 *		3. Confirm that the current value is the default value
 *		4. Confirm Set/Get work with both valid and invalid values\
 *		5. Check that the coparison operators and functions all work with 
 *		   multiple random values.
 *
 *	Note that testing is limited to random integers between -999 and 999
*/
void TestParameters::TestRangeParameter()
{
	Picto::RangeParameter param1;
	Picto::RangeParameter param2;

	int param1Min = randGen_.randInt()%900 - 1000;	//[-1000,-100]
	int param1Max = randGen_.randInt()%900 +100;		//[100,1000]
	int param1Increment = randGen_.randInt()%50 + 10;//[10,50]
	int param1Default = randGen_.randInt()%(param1Max-param1Min) + param1Min;
	param1.setIncrement(param1Increment);
	param1.setMin(param1Min);
	param1.setMax(param1Max);
	param1.setDefault(param1Default);

	//confirm that param's current value is the default that was most recently set
	QCOMPARE(param1.getValue().toInt(), (param1Default/param1Increment)*param1Increment);

	//Test set/get with valid values
	for(int i=0; i<100; i++)
	{
		int value = randGen_.randInt()%(param1Max-param1Min + 1) + param1Min;
		param1.setValue(QVariant(value));
		QCOMPARE(param1.getValue().toInt(), (value/param1Increment)*param1Increment);
	}

	//Test set/get with values that are too large
	for(int i=0; i<50; i++)
	{
		int value = randGen_.randInt()%1000 + param1Max + 1;
		param1.setValue(QVariant(value));
		QCOMPARE(param1.getValue().toInt(), (param1Max/param1Increment)*param1Increment);
	}

	//Test set/get with values that are too small
	for(int i=0; i<50; i++)
	{
		int value = param1Min - 1 - randGen_.randInt()%1000;
		param1.setValue(QVariant(value));
		QCOMPARE(param1.getValue().toInt(), (param1Min/param1Increment)*param1Increment);
	}

	//set up a second parmeter
	int param2Min = randGen_.randInt()%900 - 1000;	//[-1000,-100]
	int param2Max = randGen_.randInt()%900 +100;		//[100,1000]
	int param2Increment = randGen_.randInt()%50 + 10;//[10,50]
	int param2Default = randGen_.randInt()%(param2Max-param2Min) + param2Min;
	param2.setIncrement(param2Increment);
	param2.setMin(param2Min);
	param2.setMax(param2Max);
	param2.setDefault(param2Default);

	//test the comparison operators with random values
	for(int i=0; i<100; i++)
	{
		int value1 = randGen_.randInt()%(param1Max-param1Min) + param1Min;
		int value2 = randGen_.randInt()%(param2Max-param2Min) + param2Min;

		param1.setValue(QVariant(value1));
		param2.setValue(QVariant(value2));

		value1 = (value1/param1Increment)*param1Increment;
		value2 = (value2/param2Increment)*param2Increment;

		QCOMPARE(param1.equalTo(param2), value1 == value2);
		QCOMPARE(param1.notEqualTo(param2), value1 != value2);
		QCOMPARE(param1.greaterThan(param2), value1 > value2);
		QCOMPARE(param1.greaterThanEq(param2), value1 >= value2);
		QCOMPARE(param1.lessThan(param2), value1 < value2);
		QCOMPARE(param1.lessThanEq(param2), value1 <= value2);
		QCOMPARE(param1 == param2, value1 == value2);
		QCOMPARE(param1 != param2, value1 != value2);
		QCOMPARE(param1 > param2, value1 > value2);
		QCOMPARE(param1 >= param2, value1 >= value2);
		QCOMPARE(param1 < param2, value1 < value2);
		QCOMPARE(param1 <= param2, value1 <= value2);

		//since we aren't that likely to get equal values, let's force them
		QCOMPARE(param1.equalTo(param1), true);
		QCOMPARE(param1.notEqualTo(param1), false);
		QCOMPARE(param1 == param1, true);
		QCOMPARE(param1 != param1, false);

	}

}

/*! \brief Tests the ChoiceParameter object
 *
 *	The ChoiceParameter object is tested in the following manner:
 *		1. Create a ChoiceParameter
 *		2. Add multiple choice options to the parameter
 *		3. Test get and set
 *		4. Test comparison operators & functions
 *
 *	The object will be tested with choices that are Integer valued 
 *	and Color valued.
 */

void TestParameters::TestChoiceParameter()
{
	//Create two Choice Parameters to hold Integers
	Picto::ChoiceParameter param1;
	Picto::ChoiceParameter param2;

	param1.addChoice("1", QVariant(1));
	param1.addChoice("2", QVariant(2));
	param1.addChoice("4", QVariant(4));
	param1.addChoice("8", QVariant(8));

	param2.addChoice("1", QVariant(1));
	param2.addChoice("2", QVariant(2));
	param2.addChoice("4", QVariant(4));
	param2.addChoice("8", QVariant(8));

	//Test the setting and getting
	//Remeber that a set call only affects the parameter if the value passed in
	//is in the choice list.
	param1.setValue("4");
	int expectedVal = 4;
	for(int i=0; i<100; i++)
	{
		int currVal = randGen_.randInt(10)+1;

		if(currVal == 1 || currVal == 2 || currVal == 4 || currVal == 8)
			expectedVal = currVal;

		param1.setValue(QString("%1").arg(currVal));
		QCOMPARE(param1.getValue().toString(), QString("%1").arg(expectedVal));
	}

	//Test the comparison operators and functions using all possible combinations
	//of 1,2,4,8.  Remeber that only the == and != functionality is existing.  
	//The > and < tests will all return false, and <= >= tests will only return 
	//true if the values are equal

	for(int i=1; i<9; i*=2)
	{
		param1.setValue(QString("%1").arg(i));
		for(int j=1; j<9; j*=2)
		{
			param2.setValue(QString("%1").arg(j));

			QCOMPARE(param1.equalTo(param2), i == j);
			QCOMPARE(param1.notEqualTo(param2), i != j);
			QCOMPARE(param1.greaterThan(param2), false);
			QCOMPARE(param1.greaterThanEq(param2), i == j);
			QCOMPARE(param1.lessThan(param2), false);
			QCOMPARE(param1.lessThanEq(param2), i == j);

			QCOMPARE(param1 == param2, i == j);
			QCOMPARE(param1 != param2, i != j);
			QCOMPARE(param1 > param2, false);
			QCOMPARE(param1 >= param2, i == j);
			QCOMPARE(param1 < param2, false);
			QCOMPARE(param1 <= param2, i == j);

		}
	}



	//Create two Choice Parameters to hold Colors
	Picto::ChoiceParameter param3;
	Picto::ChoiceParameter param4;

	QStringList colorNames;
	colorNames.push_back("Red");
	colorNames.push_back("Green");
	colorNames.push_back("Blue");
	colorNames.push_back("Yellow");
	colorNames.push_back("Cyan");
	colorNames.push_back("Magenta");

	QList<QColor> colorValues;
	colorValues.push_back(QColor(255,0,0));
	colorValues.push_back(QColor(0,255,0));
	colorValues.push_back(QColor(0,0,255));
	colorValues.push_back(QColor(255,255,0));
	colorValues.push_back(QColor(0,255,255));
	colorValues.push_back(QColor(255,0,255));

	for(int i=0; i<colorNames.length(); i++)
	{
		param3.addChoice(colorNames[i], colorValues[i]);
		param4.addChoice(colorNames[i], colorValues[i]);
	}

	//Test the setting and getting with legal values
	for(int i=0; i<colorNames.length(); i++)
	{
		param3.setValue(colorNames[i]);
		QCOMPARE(param3.getValue().value<QColor>(), colorValues[i]);
	}

	//Test the setting and getting with illegal values
	//Remeber that a set call only affects the parameter if the value passed in
	//is in the choice list.
	QStringList notColors;
	notColors.push_back("red");		//wrong case
	notColors.push_back("Cy an");	//extra spaces
	notColors.push_back("Magent");	//missing last letter
	notColors.push_back("Hi, I'm really wrong");


	param3.setValue("Red");
	foreach(QString notColor, notColors)
	{
		param3.setValue(notColor);
		QCOMPARE(param3.getValue().value<QColor>(), QColor(255,0,0));
	}

	//Test the comparison operators and functions using all possible combinations
	//of colors.  Remeber that only the == and != functionality is existing.  
	//The > and < tests will all return false, and <= >= tests will only return 
	//true if the values are equal

	for(int i=0; i<colorNames.length(); i++)
	{
		param3.setValue(colorNames[i]);
		for(int j=0; j<colorNames.length(); j++)
		{
			param4.setValue(colorNames[j]);

			QCOMPARE(param3.equalTo(param4), i == j);
			QCOMPARE(param3.notEqualTo(param4), i != j);
			QCOMPARE(param3.greaterThan(param4), false);
			QCOMPARE(param3.greaterThanEq(param4), i == j);
			QCOMPARE(param3.lessThan(param4), false);
			QCOMPARE(param3.lessThanEq(param4), i == j);

			QCOMPARE(param3 == param4, i == j);
			QCOMPARE(param3 != param4, i != j);
			QCOMPARE(param3 > param4, false);
			QCOMPARE(param3 >= param4, i == j);
			QCOMPARE(param3 < param4, false);
			QCOMPARE(param3 <= param4, i == j);

		}
	}
}

/*	\brief Tests a NumericParameter object
 *
 *	Testing consists of the following:
 *		1. Create a new parameter
 *		2. Test getValue and SetValue
 *		3. Test all of the comparison operators (this implicitly tests
 *		   the comparison functions)
 */

void TestParameters::TestNumericParameter()
{
	Picto::NumericParameter param1,param2;

	int numTests = 100;

	//Test getValue and setValue
	for(int i=0; i<numTests; i++)
	{
		int val = randGen_.randInt();
		param1.setValue(QVariant(val));
		QCOMPARE(param1.getValue().toInt(),val);
	}

	//Test the comparison operators
	for(int i=0; i<numTests; i++)
	{
		int val1 = randGen_.randInt();
		int val2 = randGen_.randInt();
		param1.setValue(QVariant(val1));
		param2.setValue(QVariant(val2));

		//Testing with parameters on both sides
		QCOMPARE(param1==param2,val1==val2);
		QCOMPARE(param1!=param2,val1!=val2);
		QCOMPARE(param1<param2,val1<val2);
		QCOMPARE(param1<=param2,val1<=val2);
		QCOMPARE(param1>param2,val1>val2);
		QCOMPARE(param1>=param2,val1>=val2);
		QCOMPARE(param1==param1,val1==val1);
		QCOMPARE(param1!=param1,val1!=val1);

		//Testing with a constant on the right
		QCOMPARE(param1==QVariant(val2),val1==val2);
		QCOMPARE(param1!=QVariant(val2),val1!=val2);
		QCOMPARE(param1<QVariant(val2),val1<val2);
		QCOMPARE(param1<=QVariant(val2),val1<=val2);
		QCOMPARE(param1>QVariant(val2),val1>val2);
		QCOMPARE(param1>=QVariant(val2),val1>=val2);
		QCOMPARE(param1==QVariant(val1),val1==val1);
		QCOMPARE(param1!=QVariant(val1),val1!=val1);
	}

}