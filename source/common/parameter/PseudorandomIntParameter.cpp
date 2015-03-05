#include "PseudorandomIntParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Constructs a new PseudorandomIntParameter
 *	\details Adds a number of Properties
 *		- Value - The random value last generated with randomize().
 *		- UseSeed - Sets whether a seed is used to reproduce the random series of values predictably.
 *		- DontRepeatAnyValue - If set true, no value will be reused more than once in a row unless it 
 *			is the last value that must be used in the PseudoRandom list and has just been reshuffled.
 *		- Seed - The seed that will be used if "UseSeed" is true.
 *		- Min - The minimum integer that will be included in the list of values to be pseudorandomly selected
 *		- Max - The maximum integer that will be included in the list of values to be pseudorandomly selected
*/
PseudorandomIntParameter::PseudorandomIntParameter()
: 
  value_(0),
  units_(""),
  Parameter(),
  currIndex_(-1)
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableProperty(QVariant::Bool,"UseSeed",false);
	AddDefinableProperty(QVariant::Bool,"DontRepeatAnyValue",false);
	AddDefinableProperty(QVariant::Int,"Seed",0);
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",1);

}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* PseudorandomIntParameter::NewParameter()
{
	return new PseudorandomIntParameter;
}

/*! \brief Creates a new PseudorandomIntParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> PseudorandomIntParameter::Create()
{
	return QSharedPointer<Asset>(new PseudorandomIntParameter());
}

/*! \brief Returns the next randomly selected value from the list of values.
 *	\details The PseudorandomIntParameter works by forcing all values from the list of integers
 *	between the "Min" and "Max" parameters to be randomly selected before any of them are selected
 *	again.  This function takes care of choosing the next value within that system.  Some of the 
 *	features that it has to support are: 
 *		- Reshuffling  - If reshuffleLastValue() is called the last value to be selected with 
 *			randomize() must be selected again before the list is used up.
 *		- NonRepeating  - The same value can never show up twice in a row, even if the list
 *			of possible values was just entirely used up and we are starting the list again, and
 *			even if the last value was reshuffled.  The only exception to this is if we have used
 *			all values in the list of possible values but one.  In that case, if we reshuffle the
 *			value it must show up again next time.
 *	The way that this function deals with all of this is that it tracks a current position in
 *	an array of allowed values which is incremented at the beginning of the function.  To select a random value 
 *	it chooses a random position between its current position and the end of the array of allowed values.  
 *	The value at that position is swapped with the value at the current position and that value is used as the 
 *	random value.  This gives equal probability to all allowed values according to: 
 *	http://adrianquark.blogspot.com/2008/09/how-to-shuffle-array-correctly.html.  To deal with not repeating
 *	values, when that option is set we don't choose the last value in the array until the current index reaches it.  
 *	This assures us that when we restart a list we never pick the last value from the last time through the list
 *	twice in a row.  Since we put values that are reshuffled at the end of the array as well, they also don't appear 
 *	twice in a row.  This does of course, add some predictability to the random selection, but that is always going to be 
 *	a side effect of enforcing a rule like non-repitition on randomness.  It might be a good idea to review
 *	this algorithm again at some point in any case.
 */
void PseudorandomIntParameter::randomize()
{
	checkForPropertyChanges();

	currIndex_++;
	if(currIndex_ >= randomArray_.size())
		currIndex_ = 0;
	//Swap the first value in the array with a randomized selection of the rest of the values in the array.  
	//The method below works.  Really.  
	//See http://adrianquark.blogspot.com/2008/09/how-to-shuffle-array-correctly.html
	int swapVal = randomArray_[currIndex_];
	int maxSwapIndex = randomArray_.size()-1-currIndex_;
	if(propertyContainer_->getPropertyValue("DontRepeatAnyValue").toBool() && maxSwapIndex)
	{	//We aren't allowed to repeat the last value in the array, so we subtract 1 from the swapRange
		maxSwapIndex--;
	}
	int swapLoc = mtRand_.randInt(maxSwapIndex);	//Returns a value between 0 and the input, including zero
	randomArray_[currIndex_] = randomArray_[currIndex_+swapLoc];
	randomArray_[currIndex_+swapLoc] = swapVal;

	propertyContainer_->setPropertyValue("Value",randomArray_[currIndex_]);
}

/*! \brief Reshuffles the last randomly selected value (in randomize()) back into the list of values
 *	that need to appear before all of the random options are used up.
 *	\details We do this by swapping the latest value with the one at the back of the list and decrementing
 *	the current index value.  Decrementing the current index value would be enough to reshuffle the value, 
 *	but putting at the end of the list also takes care of assuring non-repition when that option is set.
 *	\sa randomize()
 */
void PseudorandomIntParameter::reshuffleLastValue()
{
	if(currIndex_ < 0)
	{
		//We haven't started yet.  There's nothing to reshuffle.
		return;
	}
	//Swap the last value in the array with the latest returned value and decrement the currIndex_
	//We could just decrement the current index, but swapping with the last value assures us that
	//we won't end up repeating a value if "DontRepeatAnyValue" is set (unless its the last value
	//to be used in the list, in which case we should be repeating it anyway since there's nothing
	//else to replace it).
	int lastVal = randomArray_[currIndex_];
	randomArray_[currIndex_] = randomArray_[randomArray_.size()-1];
	randomArray_[randomArray_.size()-1] = lastVal;
	if(currIndex_ == 0)
		currIndex_ = randomArray_.size()-1;
	else
		currIndex_--;
}

/*! \brief Extends Parameter::enteredScope() to resets the object to its original state as 
*	if no values have yet been randomized.
*/
void PseudorandomIntParameter::enteredScope()
{
	Parameter::enteredScope();
	//Rebuild randomArray_ from scratch.  Setting invalid values to max_ and min_
	//and calling checkForPropertyChanges() will assure that a property change is
	//detected and that the randomArray_ will be rebuilt
	max_ = -1;
	min_ = -1;
	checkForPropertyChanges();
}

/*! \brief Resets the object to its original state as if no values have yet been
 *	randomized.
 *	\details Note the method used this function supports the non-repetition option.
 */
void PseudorandomIntParameter::reset()
{
	//First we reshuffle last value.  This has the effect of putting the
	//last value at the end of the list, so it won't be repeated if 
	//"DontRepeatAnyValue" is set.
	reshuffleLastValue();
	//Set the current index to the last index in the array, so that we
	//will start over next time.
	currIndex_ = randomArray_.size()-1;
}

void PseudorandomIntParameter::postDeserialize()
{
	Parameter::postDeserialize();
	useSeed_ = propertyContainer_->getPropertyValue("UseSeed").toBool();
	seed_ = propertyContainer_->getPropertyValue("Seed").toInt();
	if(useSeed_)
		mtRand_.seed(seed_);
	min_ = propertyContainer_->getPropertyValue("Min").toInt();
	max_ = propertyContainer_->getPropertyValue("Max").toInt();
	setPropertyRuntimeEditable("Value");
}

/*! \brief Extends Parameter::validateObject() to verify that the Min Property is not greater
 *	than the Max Property.
 */
bool PseudorandomIntParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("Min").toInt() > propertyContainer_->getPropertyValue("Max").toInt())
	{
		addError("Min value cannot be greater than Max value.");
		return false;
	}
	return true;
}

bool PseudorandomIntParameter::valuesAreValid(QString& warning)
{
	int min = propertyContainer_->getProperty("Min")->initValue().toInt();
	int max = propertyContainer_->getProperty("Max")->initValue().toInt();
	int value = propertyContainer_->getProperty("Value")->initValue().toInt();
	if(min > max)
	{
		warning = "'Min' value must be less than or equal to 'Max' value.";
		return false;
	}

	if(value < min)
	{
		warning = "'Value' is less than 'Min'.";
		return false;
	}
	if(value > max)
	{
		warning = "'Value' is greater than than 'Max'.";
		return false;
	}
	return true;
}

void PseudorandomIntParameter::fixValues()
{
	int min = propertyContainer_->getProperty("Min")->initValue().toInt();
	int max = propertyContainer_->getProperty("Max")->initValue().toInt();
	int value = propertyContainer_->getProperty("Value")->initValue().toInt();
	if(min > max)
	{
		min = max;
		propertyContainer_->getProperty("Min")->setInitValue(min);
		propertyContainer_->getProperty("Value")->setInitValue(min);
	}
	if(value < min)
	{
		value = min;
		propertyContainer_->getProperty("Value")->setInitValue(value);
	}
	if(value > max)
	{
		value = max;
		propertyContainer_->getProperty("Value")->setInitValue(value);
	}
}

/*! \brief Checks if any of this object's Property values changed.  If so, selection array
 *	data variables and selection algorithm are reset to start from scratch.
*/
void PseudorandomIntParameter::checkForPropertyChanges()
{
	bool useSeed = propertyContainer_->getPropertyValue("UseSeed").toBool();
	if(useSeed)
	{
		int seed = propertyContainer_->getPropertyValue("Seed").toInt();
		if(!useSeed_ || (seed_ != seed))
		{
			//We're using a seed and weren't before, or we were before but it just changed.
			mtRand_.seed(seed);
		}
		seed_ = seed;
	}
	useSeed_ = useSeed;
	int min = propertyContainer_->getPropertyValue("Min").toInt();
	int max = propertyContainer_->getPropertyValue("Max").toInt();
	if((randomArray_.size() == 0) || (min != min_) || (max != max_))
	{	//the max or min values changed.  Rebuild the pseudorandom vector.
		max_ = max;
		min_ = min;
		randomArray_.resize(max_-min_+1);
		for(int i=min_;i<=max_;i++)
		{
			randomArray_[i-min_] = i;
		}
		//Set the current index to the last index in the array, so that we
		//will start over next time.
		currIndex_ = randomArray_.size()-1;
	}
}
/*! \brief Returns the minimum random value that may be returned from this Parameter.
 *	\details This is the value in the "Min" Property.
 */
int PseudorandomIntParameter::getMin()
{
	return propertyContainer_->getPropertyValue("Min").toInt();
}
/*! \brief Sets the minimum random value that may be returned from this Parameter.
 *	\details This is the value in the "Min" Property.
 */
void PseudorandomIntParameter::setMin(int min)
{
	if(min != min_)
	{
		propertyContainer_->setPropertyValue("Min",min);
		checkForPropertyChanges();
	}
}

/*! \brief Returns the maximum random value that may be returned from this Parameter.
 *	\details This is the value in the "Max" Property.
 */
int PseudorandomIntParameter::getMax()
{
	return propertyContainer_->getPropertyValue("Max").toInt();
}
/*! \brief Sets the maximum random value that may be returned from this Parameter.
 *	\details This is the value in the "Max" Property.
 */
void PseudorandomIntParameter::setMax(int max)
{
	if(max != max_)
	{
		propertyContainer_->setPropertyValue("Max",max);
		checkForPropertyChanges();
	}
}

} //namespace Picto