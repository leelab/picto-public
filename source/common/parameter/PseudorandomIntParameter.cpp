#include "PseudorandomIntParameter.h"

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
	int swapRange = max_-min_-currIndex_;
	if((currIndex_ == 0) && (max_-min_ > 0) && propertyContainer_->getPropertyValue("DontRepeatAnyValue").toBool())
	{	//We aren't allowed to repeat the last value in the array, so we subtract 1 from the swapRange
		swapRange--;
	}
	int swapLoc = mtRand_.randInt(swapRange);
	randomArray_[currIndex_] = randomArray_[currIndex_+swapLoc];
	randomArray_[currIndex_+swapLoc] = swapVal;

	propertyContainer_->setPropertyValue("Value",randomArray_[currIndex_]);
}

void PseudorandomIntParameter::reuseValue(QVariant value)
{
	Q_ASSERT(value.toInt() >= min_);
	Q_ASSERT(value.toInt() <= max_);
	randomArray_.push_back(value.toInt());
}

void PseudorandomIntParameter::postSerialize()
{
	Parameter::postSerialize();
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
		addError("PseudorandomIntParameter", "Min value cannot be greater than Max value.", xmlStreamReader);
		return false;
	}
	return true;
}

bool PseudorandomIntParameter::fixValues(QString&)
{
	bool returnVal = true;
	int min = propertyContainer_->getPropertyValue("Min").toInt();
	int max = propertyContainer_->getPropertyValue("Max").toInt();
	int value = propertyContainer_->getPropertyValue("Value").toInt();
	if(min > max)
	{
		min = max;
		propertyContainer_->setPropertyValue("Min",min);
		returnVal = false;
	}

	if(value < min)
	{
		value = min;
		propertyContainer_->setPropertyValue("Value",value);
		returnVal = false;
	}
	if(value > max)
	{
		value = max;
		propertyContainer_->setPropertyValue("Value",value);
		returnVal = false;
	}
	return returnVal;
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
		randomArray_.resize(max_-min+1);
		for(int i=min_;i<=max_;i++)
		{
			randomArray_[i-min_] = i;
		}
	}
}

} //namespace Picto