#include <QtWidgets>
#include "PropertyEditorFactory.h"
#include "../../common/storage/DataStore.h"
#include "../../common/property/PropertyContainer.h"
#include "ScriptWidget.h"
#include "DeletableWidget.h"
#include "PropertyEditTracker.h"
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
	editTrackers_.prepend(QSharedPointer<PropertyEditTracker>(new PropertyEditTracker(nextProp)));
	connect(editTrackers_.first().data(),SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),this,SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)));
}

//Clear should be called whenever creating more propertyWidgets if the previously created propertyWidgets
//are no longer visible.
void PropertyEditorFactory::clear()
{
	editTrackers_.clear();
}

QWidget* PropertyEditorFactory::createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent)
{
	QWidget* resultWidget = NULL;
	QString propName = property->propertyName();
	if((propName == "EntryScript")
		|| (propName == "FrameScript")
		|| (propName == "ExitScript")
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
	//Add this widget to the edit tracker for the current property so that
	//it will tell us when it is changed by the user.
	if(editTrackers_.size())
		editTrackers_.first()->addTrackedWidget(resultWidget);
	return resultWidget;
}