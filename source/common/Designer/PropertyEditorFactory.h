#ifndef PROPERTYEDITORFACTORY_H
#define PROPERTYEDITORFACTORY_H
#include <QSharedPointer>
#include <QtVariantProperty.h>
#include "EditorState.h"
#include <QMap>
#include "../common.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

/*! \brief Extends the QtVariantEditorFactory to create extra widgets needed by the Picto Designer and track the relationship
 *	between those widgets, their underlying QtProperties and the Property objects that they represent.
 *	\details The Picto Designer uses the QtPropertyBrowser system to automatically generate and display groups of widgets tailored 
 *	specifically to different types of values.  The QtVariantEditorFactory is the object within the QtPropertyBrowser
 *	system that actually takes care of generating the type specific widgets.  There are some widgets that we need that aren't 
 *	offered by the QtPropertyBrowser system like syntax highlighted textboxes for scripts and file selection buttons, so we
 *	use this class to add those to the regular QtVariantEditorFactory widgets.  Additionally, in order to get the QtPropertyBrowser
 *	system to work with Picto Property objects instead of its own QtProperty objects, we need to go through some roundabout
 *	logic where we keep lookup tables of Property objects indexed by the QtProperty objects representing them, then tell the world
 *	when a widget value was edited and use the lookup table to figure out which Property we should say was represented by the edited widget.  
 *	This class takes care of all of these issues.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyEditorFactory : public QtVariantEditorFactory
#else
class PropertyEditorFactory : public QtVariantEditorFactory
#endif
{
    Q_OBJECT

public:
   PropertyEditorFactory(QWidget *parent=0);
   PropertyEditorFactory(QSharedPointer<EditorState> editorState, QWidget *parent=0);
   virtual ~PropertyEditorFactory(){};
   void setNextProperty(QSharedPointer<Property> nextProp);
   void clear();
signals:
   	/*! \brief Emitted when the prop Property's value was edited.  val is the value which should now be set to it.
	 *	\note Some outside receiver of this signal must actually set val to the prop Property.  That is not handled
	 *	by this widget.
	 */
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);

protected:
	virtual QWidget* createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent);
private:
	QSharedPointer<Property> nextProp_;
	QSharedPointer<EditorState> editorState_;
	QHash<QtProperty*,QSharedPointer<Property>> qtpropToPropMap_;
	QHash<QtVariantPropertyManager*,bool> trackedPropManagers_;

private slots:
	void qtPropValueChanged(QtProperty* property,const QVariant& value);


};

#endif
