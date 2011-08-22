#include "Parameter.h"

namespace Picto {

Parameter::Parameter() :
	fixingValues_(false)
{
}

//void Parameter::reset()
//{
//	setValue(propertyContainer_->getPropertyValue("Value"));
//}

//void Parameter::setInitialValue(QVariant val)
//{
//	propertyContainer_->setPropertyValue("Value",verifyValue(val));
//	//PUT IN VERIFY VALUE BUT WITH INPUT VALUE SO THAT THIS CAN BE VERIFIED TO!!!
//}

//void Parameter::setValue(QVariant value)
//{
//	propertyContainer_->setPropertyValue("Value",verifyValue(value));
//}
//
//QVariant Parameter::getValue()
//{
//	return propertyContainer_->getPropertyValue("Value");
//}

void Parameter::postSerialize()
{
	Scriptable::postSerialize();
	connect(this,SIGNAL(edited()),this,SLOT(valueEdited()));
	//setInitialValue(propertyContainer_->getPropertyValue("Value"));
	//reset();
}

void Parameter::valueEdited()
{
	if(fixingValues_)
		return;
	fixingValues_ = true;
	QString warning;
	if(!fixValues(warning))
	{
		if(!warning.isEmpty())
		{
			//DO SOMETHING!
		}
	}
	fixingValues_ = false;
}

}; //namespace Picto
