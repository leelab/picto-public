#include "ChoiceParameter.h"
#include "../Storage/PropertyFactory.h"

namespace Picto {

ChoiceParameter::ChoiceParameter()
{
	
	AddDefinableProperty(QVariant::Bool,"OperatorUI",false);
	AddDefinableProperty(QVariant::Int,"Order",0);
	AddDefinableProperty("Choice","");

	QSharedPointer<AssetFactory> factory(new AssetFactory());
	factory->addAssetType("Bool",QSharedPointer<AssetFactory>
		(new PropertyFactory(0,-1,propertyContainer_,QVariant::Bool,"Bool")));
	factory->addAssetType("Int",QSharedPointer<AssetFactory>
		(new PropertyFactory(0,-1,propertyContainer_,QVariant::Int,"Int")));
	factory->addAssetType("Double",QSharedPointer<AssetFactory>
		(new PropertyFactory(0,-1,propertyContainer_,QVariant::Double,"Double")));
	factory->addAssetType("String",QSharedPointer<AssetFactory>
		(new PropertyFactory(0,-1,propertyContainer_,QVariant::String,"String")));
	factory->addAssetType("Color",QSharedPointer<AssetFactory>
		(new PropertyFactory(0,-1,propertyContainer_,QVariant::Color,"Color")));
	AddDefinableObjectFactory("Option",factory);

	type_ = "Choice";
	currentOption_ = "";
	defaultOption_ = "";
}

Parameter* ChoiceParameter::NewParameter()
{
	return new ChoiceParameter;
}

/*!	\brief Adds a new choice to the end of the list */
bool ChoiceParameter::addChoice(QString label, QVariant data)
{
	//confirm that a choice with this label doesn't already exist
	if(options_.contains(label))
		return false;

	ChoiceParameterOption option;
	option.type = data.typeName();
	option.data = data;

	options_[label] = option;
	return true;
}

/*! \brief Removes a choice from the list.  Returns false if the passed in String isn't in the list */
bool ChoiceParameter::removeChoice(QString label)
{
	if(!options_.contains(label))
		return false;

	options_.remove(label);
	return true;
}

void ChoiceParameter::setDefaultOption(QString label)
{
	defaultOption_ = label;
}


/* \brief Sets the current value to the passed in string
 *
 *	Searches through the choices and finds the choice with a 
 *	matching label and makes that the current choice
 *	NOTE: This function is case sensitive!
 */
void ChoiceParameter::setValue(QVariant value)
{
	if(options_.contains(value.toString()))
		currentOption_ = value.toString();
}

//Returns the current value as a string
QVariant ChoiceParameter::getValue()
{
	if(options_.isEmpty() || !options_.contains(currentOption_))
		return QVariant("");
	else
		return options_[currentOption_].data;
}

/*!	\brief Equal to operation
 *
 *	Returns true if the passed in parameter's current choice data is equal
 *	to our current choice.  Note that this only compares the data, so there
 *	could be some weird issues...
 */
bool ChoiceParameter::equalTo(Parameter& RHS)
{
	return options_[currentOption_].data == RHS.getValue();
}

/*! \brief Equal to operation with a constant as RHS
 *
 *	There are a number of different types of constants that might be passed in
 *	and we can handle all of them.  If a string is passed in, we check to see 
 *	if it matches the label in the current choice.  If that fails (either because
 *	the passed in value isn't a string, or it doesn't match), we check to see if 
 *	it matches the type and value of the current data.
 */
bool ChoiceParameter::equalTo(QVariant& RHS)
{
	//check the passed in value to see if it matches our current label
	if(RHS.type() == QVariant::String &&
		RHS.toString() == currentOption_)
		return true;

	//check the passed in value to see if it matches our current data (in 
	//type and value)
	if(RHS.typeName() == options_[currentOption_].type)
	{
		if(options_[currentOption_].data == RHS)
			return true;
	}
	return false;
}

void ChoiceParameter::postSerialize()
{
	Parameter::postSerialize();
	//We should clear out the option list in cse this isn't a 
	//fresh instance
	options_.clear();

	bOperatorUI_ = propertyContainer_->getPropertyValue("OperatorUI").toBool();
	order_ = propertyContainer_->getPropertyValue("Order").toInt();
	defaultOption_ = propertyContainer_->getPropertyValue("Default").toBool();

	ChoiceParameterOption newOption;
	QList<QSharedPointer<Asset>> options = getGeneratedChildren("Option");
	foreach(QSharedPointer<Asset> option,options)
	{
		newOption.data = option.staticCast<Property>()->value();
		newOption.type = newOption.data.type();
		options_[option.staticCast<Property>()->getName()] = newOption;
	}
}

bool ChoiceParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
