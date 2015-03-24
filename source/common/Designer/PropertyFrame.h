#ifndef PROPERTYFRAME_H
#define PROPERTYFRAME_H
#include "PropertyGroupWidget.h"
#include "../common/experiment/experiment.h"
#include <QScrollArea>
#include <QSqlDatabase>
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

/*! \brief Displays a list of widgets for RuntimeEditable Properties to be used for updating Property initValues during an
 *	experimental run.
 *	\details During the course of an experimental run, we often want to change certain Property values in the experiment
 *	to help in training the user or simply perform some kind of test.  In Picto, this is accomplished by updating
 *	Properties initValues.  Since each Property value is reset to its initValue each time it enters scope, changing a
 *	Property's initValue assures that its runValue will  be updated at a defined point in the control flow.  Additionally,
 *	we don't want to show every Property initValue to the operator during an experiment, so we use "UIEnabled" checkboxes
 *	in the Design to allow the designer to choose which DesignElements display Property Widgets during an experiment.  We
 *	also don't want to make every Property of every UIEnabled Element show up, so only Properties defined as RuntimeEditable
 *	in the code are shown.  This widget goes through the Design Elements underneath an Element input into
 *	setTopLevelDataStore.  It finds all UIEnabled elements, extracts their RuntimeEditable Properties, and displays them
 *	in groups (labelled by the UIEnabled Element name) of widgets.  When the widget for a particular Property is edited,
 *	the Property's initValue is changed, and a parameterMessageReady() signal is triggered to inform outside objects that
 *	this value has been changed.  When running a remote experiment, this allows the RemoteViewer to capture the changed
 *	initValue and send it over the network to the corresponding Property in the running Experiment.
 *
 *	This class uses the PropertyGroupWidget to handle Property widget creation and layout.  It is responsible for finding
 *	and organizing the RuntimeEditable Properties and loading them into the PropertyGroupWidget.
 *
 * `This class also includes an interface for updating Property initValues based on values recorded in a previous session.
 *	\sa DataStore::setPropertyRuntimeEditable(), Property::setRuntimEditable()
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyFrame : public QScrollArea
#else
class PropertyFrame : public QScrollArea
#endif
{
    Q_OBJECT

public:
   PropertyFrame(QWidget *parent=0);
   virtual ~PropertyFrame(){};
   void clearProperties(){setTopLevelDataStore(QSharedPointer<DataStore>());};
   void setTopLevelDataStore(QSharedPointer<DataStore> dataStore);
   void updatePropertiesFromFile(QString filename);
signals:
   /*!	\brief Indicates that the changedProp Property's initValue has changed. 
    *	\note This is clearly a bad name.  It was named when we used to refer to the Properties in this widget as
	*	parameters and when this widget was only used with the RemoteViewer that sends messages over the network.  This
	*	name should probably be changed.
    */
   void parameterMessageReady(QSharedPointer<Property> changedProp);
private:
	QWidget* mainWidget_;
	QSqlDatabase propValsSession_;
	QHash<QString,QSharedPointer<Property>> pathMap_;
private slots:
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);
};

#endif
