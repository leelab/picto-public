#include <QtWidgets>
#include "PropertyEditorFactory.h"
#include "../../common/storage/DataStore.h"
#include "../../common/property/PropertyContainer.h"
#include "ScriptWidget.h"
//#include "DeletableWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyEditorFactory::PropertyEditorFactory(QWidget *parent) :
	QtVariantEditorFactory(parent)
{

}

//Sets the next property who's QtProperties will be requested in create editor
//This should be called just before callind a propertybrowser's addProperty function
//with a Property's QtProperty.  It tells this object which Property the next QtProperty
//is associated with, allowing that property to be included in propertyEdited signals.
void PropertyEditorFactory::setNextProperty(QSharedPointer<Picto::Property> nextProp)
{
	nextProp_ = nextProp;
}

//Clear should be called whenever creating more propertyWidgets if the previously created propertyWidgets
//are no longer visible.
void PropertyEditorFactory::clear()
{
	trackedPropManagers_.clear();
	qtpropToPropMap_.clear();
}

QWidget* PropertyEditorFactory::createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent)
{
	//If this is the first qtProp for the latest Picto prop, it is the top level prop.
	//If this is for a Script property, don't associate this top level QtProperty with the latest Picto Property.
	//If this is any other type, this top level QtProperty should be associated with the latest Picto Property
	if(static_cast<QtVariantProperty*>(property)->propertyType() != QtVariantPropertyManager::groupTypeId())
	{
		if(nextProp_)
			qtpropToPropMap_[property] = nextProp_;
		nextProp_.clear();
	}
	//If we haven't connected this managers "property changed signal" to our slot yet, do it.
	if(!trackedPropManagers_.contains(manager))
	{
		trackedPropManagers_[manager] = true;
		connect(manager,SIGNAL(valueChanged (QtProperty*, const QVariant&)),this,SLOT(qtPropValueChanged(QtProperty*,const QVariant&)));
	}

	QWidget* resultWidget = NULL;
	QString propName = property->propertyName();
	if((propName == "")
		|| (propName == "Script")
		|| (propName == "PausingScript")
		|| (propName == "RestartingScript"))
	{
		resultWidget = new ScriptWidget(manager,property,parent);
		resultWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	}
	else
	{
		resultWidget = QtVariantEditorFactory::createEditor(manager,property,parent);
		if(resultWidget)
			resultWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	}

	if(!resultWidget)
		return NULL;

	//Change default widget behavior for doubles
	if(resultWidget->inherits("QDoubleSpinBox"))
	{
		qobject_cast<QDoubleSpinBox*>(resultWidget)->setDecimals(4);
		qobject_cast<QDoubleSpinBox*>(resultWidget)->setSingleStep(.01);
		qobject_cast<QDoubleSpinBox*>(resultWidget)->setValue(manager->value(property).toDouble());
	}


	return resultWidget;
}

//This is called whenever a QtProperty value changes.  It translates the message and emits
//a signal indicating which property's value is supposed to be changed, and to what value.
void PropertyEditorFactory::qtPropValueChanged(QtProperty* property,const QVariant& value)
{
	if(!qtpropToPropMap_.contains(property))
		return;
	emit propertyEdited(qtpropToPropMap_.value(property),value);
}