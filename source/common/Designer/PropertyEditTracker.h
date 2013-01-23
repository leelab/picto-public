#ifndef PropertyEditTracker_H
#define PropertyEditTracker_H
#include <QComboBox>
#include <QtVariantProperty.h>
#include "../../common/property/property.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyEditTracker : public QObject
{
    Q_OBJECT

public:
	PropertyEditTracker(QSharedPointer<Picto::Property> prop);
	virtual ~PropertyEditTracker(){};
	void addTrackedWidget(QWidget* widget);
signals:
	void propertyEdited(QSharedPointer<Property> prop);
protected:
	QSharedPointer<Picto::Property> prop_;
private:
	bool textEdited_;
	int lastHighlightedNewValue_;	//This will be -2 if the last highlighted value isn't new
	QComboBox* comboBoxWidget_;	//We need to maintain a pointer to the combobox widget as part of its change tracking system.
private slots:
	//We need to know when the value in a property changed AS A RESULT OF ACTIONS THROUGH THE
	//PROPERTY BROWSER.  For this purpose, we look at all the varying types of events from 
	//the various EditBoxes and translate them into one unified propertyEdited()
	//signal
	void valueEdited(int){emit propertyEdited(prop_);};
	void valueEdited(double){emit propertyEdited(prop_);};
	void valueEdited(const QString &);
	void valueEdited(){emit propertyEdited(prop_);};
	void valueEdited(QColor){emit propertyEdited(prop_);};
	void lineEditFinished();
	void comboBoxEntryHighlighted(int index);
	void comboBoxEntrySelected(int index);

};

//class StringPropertyEditTracker : public PropertyEditTracker
//{
//	Q_OBJECT
//public:
//	StringPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent=0);
//};
//
//class IntPropertyEditTracker : public PropertyEditTracker
//{
//	Q_OBJECT
//public:
//	IntPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent=0);
//};
//
//class BoolPropertyEditTracker : public PropertyEditTracker
//{
//	Q_OBJECT
//public:
//	BoolPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent=0);
//};
//
//class EnumPropertyEditTracker : public PropertyEditTracker
//{
//	Q_OBJECT
//public:
//	EnumPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent=0);
//};
//
//class ColorPropertyEditTracker : public PropertyEditTracker
//{
//	Q_OBJECT
//public:
//	ColorPropertyEditTracker(QtVariantPropertyManager* manager, QtProperty* qtProp, QSharedPointer<Picto::Property> prop, QWidget *parent=0);
//};



//! [0]
#endif
