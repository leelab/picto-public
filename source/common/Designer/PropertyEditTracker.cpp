#include <QtWidgets>
#include "PropertyEditTracker.h"
#include "ScriptWidget.h"
#include "../../common/memleakdetect.h"

//! [0]
PropertyEditTracker::PropertyEditTracker(QSharedPointer<Picto::Property> prop) :
	prop_(prop),
	textEdited_(false),
	lastHighlightedNewValue_(-2),
	trackedWidget_(NULL)
{
}

void PropertyEditTracker::addTrackedWidget(QWidget* widget, QtVariantProperty* qtProp)
{
	trackedWidget_ = widget;
	qtProp_ = qtProp;
	QList<QSpinBox*> spinKids = trackedWidget_->findChildren<QSpinBox*>();
	QList<QDoubleSpinBox*> doubleSpinKids = trackedWidget_->findChildren<QDoubleSpinBox*>();
	QList<QCheckBox*> checkKids = trackedWidget_->findChildren<QCheckBox*>();
	QList<QLineEdit*> lineKids = trackedWidget_->findChildren<QLineEdit*>();
	QList<ScriptWidget*> scriptKids = trackedWidget_->findChildren<ScriptWidget*>();
	if(trackedWidget_->inherits("QSpinBox"))
	{
		lineKids.clear();//Spinboxes include lineedits and we don't want the signal twice
		spinKids.append(static_cast<QSpinBox*>(trackedWidget_));
	}
	else if(trackedWidget_->inherits("QDoubleSpinBox"))
	{
		lineKids.clear();//DoubleSpinboxes include lineedits and we don't want the signal twice
		doubleSpinKids.append(static_cast<QDoubleSpinBox*>(trackedWidget_));
	}
	else if(trackedWidget_->inherits("QCheckBox"))
		checkKids.append(static_cast<QCheckBox*>(trackedWidget_));
	else if(trackedWidget_->inherits("QLineEdit"))
		lineKids.append(static_cast<QLineEdit*>(trackedWidget_));
	else if(trackedWidget_->inherits("ScriptWidget"))
		scriptKids.append(static_cast<ScriptWidget*>(trackedWidget_));
	else if(trackedWidget_->inherits("QtColorEditWidget"))
	{	//Since we can't actually include QtColorEditWidget (its in a cpp file
		//c:\qtpropertybrowser-2.5-opensource\src\qteditorfactory.cpp)
		//we use this convoluded way of connecting to its valueChanged signal
		//since it is never the child of another widget, so we can.
		connect(trackedWidget_,SIGNAL(valueChanged(QColor)),this,SLOT(valueEdited(QColor)));
	}
	else if(trackedWidget_->inherits("QComboBox"))
	{	//Combo boxes also have a funny interface.  They have a signal that says 
		//that a user selection occured that gets triggered even if nothing changed.
		//They also have a signal that says what a user highlights.  We use these
		//to see if the user changed something by checking if the last highlighted 
		//value was different from the current property value and the same as the
		//final selection.  This doesn't work if there is more than one ComboBox for
		//a property, but luckily, there isn't.
		connect(trackedWidget_,SIGNAL(highlighted(int)),this,SLOT(comboBoxEntryHighlighted(int)));
		connect(trackedWidget_,SIGNAL(activated(int)),this,SLOT(comboBoxEntrySelected(int)));
	}

	foreach(QSpinBox* kid, spinKids)
	{
		connect(kid,SIGNAL(valueChanged(int)),this,SLOT(valueEdited(int)));
	}
	foreach(QDoubleSpinBox* kid, doubleSpinKids)
	{
		connect(kid,SIGNAL(valueChanged(double)),this,SLOT(valueEdited(double)));
	}	
	foreach(QCheckBox* kid, checkKids)
	{
		connect(kid,SIGNAL(stateChanged(int)),this,SLOT(valueEdited(int)));
	}
	foreach(QLineEdit* kid, lineKids)
	{
		connect(kid,SIGNAL(textEdited(const QString &)),this,SLOT(valueEdited(const QString &)));
		connect(kid,SIGNAL(editingFinished()),this,SLOT(lineEditFinished()));
	}
	foreach(ScriptWidget* kid, scriptKids)
	{
		connect(kid,SIGNAL(textEdited(const QString &)),this,SLOT(valueEdited(const QString &)));
		connect(kid,SIGNAL(editingFinished()),this,SLOT(lineEditFinished()));
	}
}

void PropertyEditTracker::valueEdited(int val)
{
	//emit propertyEdited(prop_,qtProp_->value());
}

void PropertyEditTracker::valueEdited(double val)
{
	//emit propertyEdited(prop_,qtProp_->value());
}

//Since textChanged is textEdited is called whenever any text changes (ie. a single letter)
//we don't want to register that level of detail.  We use this function to track whether
//text changed in a lineedit before editingFinished() is called (signifying that the user 
//pressed enter or changed focus
void PropertyEditTracker::valueEdited(const QString &val)
{
	textEdited_ = true;
	latestEditedText_ = val;
}

void PropertyEditTracker::valueEdited(QColor val)
{
	//emit propertyEdited(prop_,qtProp_->value());
}

void PropertyEditTracker::lineEditFinished()
{
	if(textEdited_)
	{
		//emit propertyEdited(prop_,qtProp_->value());
	}
	textEdited_ = false;
	latestEditedText_ = "";
}

void PropertyEditTracker::comboBoxEntryHighlighted(int index)
{
	if(index == qobject_cast<QComboBox*>(trackedWidget_)->currentIndex())
		lastHighlightedNewValue_ = -2;	//-2 indicates that the last highlighted value was not new.
	else
		lastHighlightedNewValue_ = index;
}
void PropertyEditTracker::comboBoxEntrySelected(int index)
{
	//If the input index==lastHighlightedNewValue_ then a new value was selected
	//(since index will never be -2)
	if(index == lastHighlightedNewValue_)
		//emit propertyEdited(prop_,qtProp_->value());
	lastHighlightedNewValue_ = -2;	//If we're here, the combobox isn't being used anymore, so reset the lastHighlightedNewValue_.
}