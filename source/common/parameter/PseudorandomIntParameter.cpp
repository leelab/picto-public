#include "PseudorandomIntParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

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

Parameter* PseudorandomIntParameter::NewParameter()
{
	return new PseudorandomIntParameter;
}

QSharedPointer<Asset> PseudorandomIntParameter::Create()
{
	return QSharedPointer<Asset>(new PseudorandomIntParameter());
}

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

//Resets the object to its original state as if no values have yet been
//randomized.
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

//Resets the object to its original state as if no values have yet been
//randomized.
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
	currValue_ = min_-1;
	setPropertyRuntimeEditable("Value");
}

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
int PseudorandomIntParameter::getMin()
{
	return propertyContainer_->getPropertyValue("Min").toInt();
}
void PseudorandomIntParameter::setMin(int min)
{
	if(min != min_)
	{
		propertyContainer_->setPropertyValue("Min",min);
		checkForPropertyChanges();
	}
}
int PseudorandomIntParameter::getMax()
{
	return propertyContainer_->getPropertyValue("Max").toInt();
}
void PseudorandomIntParameter::setMax(int max)
{
	if(max != max_)
	{
		propertyContainer_->setPropertyValue("Max",max);
		checkForPropertyChanges();
	}
}

} //namespace Picto