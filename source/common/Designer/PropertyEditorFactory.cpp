#include <QtWidgets>
#include "PropertyEditorFactory.h"
#include "../../common/storage/DataStore.h"
#include "../../common/property/PropertyContainer.h"
#include "../parameter/AssociateElement.h"
#include "ScriptWidget.h"
#include "FileSelectWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


PropertyEditorFactory::PropertyEditorFactory(QWidget *parent) :
	QtVariantEditorFactory(parent) 
{

}

/*! \brief Constructs a new PropertyEditorFactory.
 *	\details The EditorState is needed in the constructor of searchable widgets as well as in order
 *	to detect whether there is an active analysis so that non-analysis Properties can be set as read-only.
 */
PropertyEditorFactory::PropertyEditorFactory(QSharedPointer<EditorState> editorState, QWidget *parent) :
	QtVariantEditorFactory(parent),
	editorState_(editorState) 
{

}

/*! \brief Sets the next Property who's QtProperties will be requested in create editor.
 *	\details This should be called just before callind a QtPropertyBrowser's addProperty function.  
 *	It tells this object which Property the next QtProperty is associated with, allowing the PropertyEditorFactory 
 *	to look up that Property when QtProperty values are edited by widgets so that the Property can be included in 
 *	the propertyEdited() signal.
 */
void PropertyEditorFactory::setNextProperty(QSharedPointer<Picto::Property> nextProp)
{
	nextProp_ = nextProp;
}

/*! \brief Whenever we want to create more Property Widgets, if previously created Property Widgets are no longer visible, clear
 *	should be called first to remove data tracked from previous widgets.
 */
void PropertyEditorFactory::clear()
{
	trackedPropManagers_.clear();
	qtpropToPropMap_.clear();
}

/*! \brief Creates an editor widget for the input QtProperty that was created by the input manager.
 *	\details This function extends the QtVariantEditorFactory::createEditor() function
 *	to generate some specific widgets needed for Picto, configure generated widgets for the current Designer mode,
 *	and store which Picto Property objects are related to which QtProperty objects.
 *	\sa setNextProperty()
 */
QWidget* PropertyEditorFactory::createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent)
{
	//Check if this property needs to be read only
	bool isReadOnly = false;
	if(nextProp_ && !nextProp_->isGuiEditable())
		isReadOnly = true;

	//If there is an active analysis, and the currently selected asset is not an analysis asset, set all properties as read only (we will take care of AnalysisScripts later).
	if(editorState_ && editorState_->getSelectedAsset())
	{
		AssociateElement* assocElem = dynamic_cast<AssociateElement*>(editorState_->getSelectedAsset().data());
		if(!editorState_->getCurrentAnalysis().isNull() && !assocElem)
		{	
			isReadOnly = true;
		}
	}
	//If this is the first qtProp for the latest Picto prop, it is the top level prop.
	//If this is for a Script property, don't associate this top level QtProperty with the latest Picto Property.
	//If this is any other type, this top level QtProperty should be associated with the latest Picto Property
	QString propName = property->propertyName();
	if(static_cast<QtVariantProperty*>(property)->propertyType() != QtVariantPropertyManager::groupTypeId())
	{
		if(nextProp_)
		{
			//If this is for an analysis script, it should never be read only
			if((propName.isEmpty()) && nextProp_->getName().left(8) == "Analysis")
				isReadOnly = false;
			qtpropToPropMap_[property] = nextProp_;
		}
		nextProp_.clear();
	}
	//If we haven't connected this managers "property changed signal" to our slot yet, do it.
	if(!trackedPropManagers_.contains(manager))
	{
		trackedPropManagers_[manager] = true;
		connect(manager,SIGNAL(valueChanged (QtProperty*, const QVariant&)),this,SLOT(qtPropValueChanged(QtProperty*,const QVariant&)));
	}

	QWidget* resultWidget = NULL;
	if((propName == "") || (propName == "Script"))
	{
		Q_ASSERT(editorState_);
		resultWidget = new ScriptWidget(manager,property,editorState_,false,parent);
		resultWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	}
	else if(propName == "ControlTarget")
	{
		Q_ASSERT(editorState_);
		resultWidget = new ScriptWidget(manager,property,editorState_,true,parent);
	}
	else if(propName == "ImageFile")
	{
		Q_ASSERT(editorState_);
		resultWidget = new ScriptWidget(manager,property,editorState_,true,parent);
	}
	else if(propName == "FileName")
	{
		Q_ASSERT(editorState_);
		resultWidget = new FileSelectWidget(manager,property,editorState_,parent);
	}
	else if (static_cast<QtVariantProperty*>(property)->propertyType() == QVariant::String)
	{
		//Q_ASSERT(editorState_);
		resultWidget = new ScriptWidget(manager, property, editorState_, true, parent);
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

	if(isReadOnly)
	{
		if(resultWidget->inherits("QLineEdit"))
			qobject_cast<QLineEdit*>(resultWidget)->setReadOnly(true);	
		if(resultWidget->inherits("QDoubleSpinBox"))
			qobject_cast<QDoubleSpinBox*>(resultWidget)->setReadOnly(true);
		if(resultWidget->inherits("QSpinBox"))
			qobject_cast<QSpinBox*>(resultWidget)->setReadOnly(true);
		if(resultWidget->inherits("ScriptWidget"))
			qobject_cast<ScriptWidget*>(resultWidget)->setReadOnly(true);
	}

	return resultWidget;
}

/*! \brief Called whenever a QtProperty value changes.  Looks up the Property represented by this QtProperty and emits propertyEdited with
 *	that Property and the value to which is should be changed.
 *	\sa setNextProperty()
 */
void PropertyEditorFactory::qtPropValueChanged(QtProperty* property,const QVariant& value)
{
	if(!qtpropToPropMap_.contains(property))
		return;
	emit propertyEdited(qtpropToPropMap_.value(property),value);
}