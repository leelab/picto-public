#include <QtGui>
#include "PropertyEditTracker.h"
#include "ScriptWidget.h"
#include "../../common/memleakdetect.h"

//! [0]
PropertyEditTracker::PropertyEditTracker(QSharedPointer<Picto::Property> prop) :
	prop_(prop),
	textEdited_(false),
	lastHighlightedNewValue_(-2),
	comboBoxWidget_(NULL)
{
}

void PropertyEditTracker::addTrackedWidget(QWidget* widget)
{
	QList<QSpinBox*> spinKids = widget->findChildren<QSpinBox*>();
	QList<QDoubleSpinBox*> doubleSpinKids = widget->findChildren<QDoubleSpinBox*>();
	QList<QCheckBox*> checkKids = widget->findChildren<QCheckBox*>();
	QList<QLineEdit*> lineKids = widget->findChildren<QLineEdit*>();
	QList<ScriptWidget*> scriptKids = widget->findChildren<ScriptWidget*>();
	if(widget->inherits("QSpinBox"))
	{
		lineKids.clear();//Spinboxes include lineedits and we don't want the signal twice
		spinKids.append(static_cast<QSpinBox*>(widget));
	}
	else if(widget->inherits("QDoubleSpinBox"))
	{
		lineKids.clear();//DoubleSpinboxes include lineedits and we don't want the signal twice
		doubleSpinKids.append(static_cast<QDoubleSpinBox*>(widget));
	}
	else if(widget->inherits("QCheckBox"))
		checkKids.append(static_cast<QCheckBox*>(widget));
	else if(widget->inherits("QLineEdit"))
		lineKids.append(static_cast<QLineEdit*>(widget));
	else if(widget->inherits("ScriptWidget"))
		scriptKids.append(static_cast<ScriptWidget*>(widget));
	else if(widget->inherits("QtColorEditWidget"))
	{	//Since we can't actually include QtColorEditWidget (its in a cpp file
		//c:\qtpropertybrowser-2.5-opensource\src\qteditorfactory.cpp)
		//we use this convoluded way of connecting to its valueChanged signal
		//since it is never the child of another widget, so we can.
		connect(widget,SIGNAL(valueChanged(QColor)),this,SLOT(valueEdited(QColor)));
	}
	else if(widget->inherits("QComboBox"))
	{	//Combo boxes also have a funny interface.  They have a signal that says 
		//that a user selection occured that gets triggered even if nothing changed.
		//They also have a signal that says what a user highlights.  We use these
		//to see if the user changed something by checking if the last highlighted 
		//value was different from the current property value and the same as the
		//final selection.  This doesn't work if there is more than one ComboBox for
		//a property, but luckily, there isn't.
		connect(widget,SIGNAL(highlighted(int)),this,SLOT(comboBoxEntryHighlighted(int)));
		connect(widget,SIGNAL(activated(int)),this,SLOT(comboBoxEntrySelected(int)));
		comboBoxWidget_ = qobject_cast<QComboBox*>(widget);
		
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
		connect(kid,SIGNAL(editingFinishedAndTextEdited()),this,SLOT(valueEdited()));
	}
}

//Since textChanged is textEdited is called whenever any text changes (ie. a single letter)
//we don't want to register that level of detail.  We use this function to track whether
//text changed in a lineedit before editingFinished() is called (signifying that the user 
//pressed enter or changed focus
void PropertyEditTracker::valueEdited(const QString &)
{
	textEdited_ = true;
}

void PropertyEditTracker::lineEditFinished()
{
	if(textEdited_)
		emit propertyEdited(prop_);
	textEdited_ = false;
}

void PropertyEditTracker::comboBoxEntryHighlighted(int index)
{
	if(index == comboBoxWidget_->currentIndex())
		lastHighlightedNewValue_ = -2;	//-2 indicates that the last highlighted value was not new.
	else
		lastHighlightedNewValue_ = index;
}
void PropertyEditTracker::comboBoxEntrySelected(int index)
{
	//If the input index==lastHighlightedNewValue_ then a new value was selected
	//(since index will never be -2)
	if(index == lastHighlightedNewValue_)
		emit propertyEdited(prop_);
	lastHighlightedNewValue_ = -2;	//If we're here, the combobox isn't being used anymore, so reset the lastHighlightedNewValue_.
}



//
//
//
//
//StringPropertyEditTracker::StringPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent) :
//PropertyEditTracker(manager,qtProp,prop,parent)
//{
//	QLineEdit* edit = new QLineEdit(this);
//	edit->setText(getPropValue().toString());
//}
//IntPropertyEditTracker::IntPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent) :
//PropertyEditTracker(manager,qtProp,prop,parent)
//{
//	QSpinBox* edit = new QSpinBox(this);
//	edit->setValue(getPropValue().toInt());
//}
//BoolPropertyEditTracker::BoolPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent) :
//PropertyEditTracker(manager,qtProp,prop,parent)
//{
//	QCheckBox* edit = new QCheckBox(this);
//	edit->setChecked(getPropValue().toBool());
//}
//EnumPropertyEditTracker::EnumPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent) :
//PropertyEditTracker(manager,qtProp,prop,parent)
//{
//	QComboBox* edit = new QComboBox(this);
//	QStringList enumNames = manager_->attributeValue(qtProp_,"enumNames").toStringList();
//	edit->addItems(enumNames);
//	edit->setCurrentIndex(getPropValue().toInt());
//}
//ColorPropertyEditTracker::ColorPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent) :
//PropertyEditTracker(manager,qtProp,prop,parent)
//{
//	QTextEdit* edit = new QTextEdit(this);
//	edit->setText(getPropValue().toString());
//}
