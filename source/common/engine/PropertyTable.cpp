#include "PropertyTable.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new PropertyTable.  
 *	\details designConfig is used to access Properties by Asset ID instead of having to lookup them up by their Experimental Path.
*/
PropertyTable::PropertyTable(QSharedPointer<DesignConfig> designConfig)
: designConfig_(designConfig)
{
	Q_ASSERT(designConfig_);
}
/*! \brief Adds the input Property for inclusion in this PropertyTable's propertyValueChanged()/propertyInitValueChanged() signals.
 *	\details Only Properties added using this function will cause one of the valueChanged() signals to be emitted
 *	when their values change.
 */
void PropertyTable::addProperty(QSharedPointer<Property> prop)
{
	connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChange(Property*,QVariant)));
	connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propInitValueChange(Property*,QVariant)));
}

/*! \brief Causes propertyValueChanged() and propertyInitValueChanged() signals to be emitted for every property in the current Experiment.
 *	\details This can be used in conjunction with the Picto Engine to send all current property values to server.
 *	\note This function is set to ignore Transitions and Control Links.  Transitions and Control Links don't have any Properties 
 *	that can change during and experiment.  
 *	\note Whereas in addProperty() we wrote that only those Properties added through that function would participate in valueChanged() signal
 *	emission.  That was with regard to when the Property values actually change.  In this function, we emit the valueChanged() signals regardless
 *	of what was or was not added in addProperty().
 */
void PropertyTable::reportChangeInAllProperties()
{
	foreach(QWeakPointer<Asset> prop,designConfig_->getProperties())
	{
		QSharedPointer<Asset> sAsset(prop);
		if(!sAsset)
			continue;
		QSharedPointer<Asset> propParent = sAsset->getParentAsset();
		if(	propParent 
			&& (	
				propParent->inherits("Picto::Transition") 
				||	propParent->inherits("Picto::ControlLink")
			)
		)
		continue;
		emit propertyValueChanged(sAsset.staticCast<Property>().data());
		emit propertyInitValueChanged(sAsset.staticCast<Property>().data());
	}
}

/*! \brief Sets the initValue of the Property with Asset ID equal to index to value.
 *	\details We assume here that the value was created using initValToUserString().
*/
void PropertyTable::updateInitPropertyValue(int index,QString value)
{
	QSharedPointer<Asset> asset = designConfig_->getAsset(index);
	Q_ASSERT(asset->inherits("Picto::Property"));
	asset.staticCast<Property>()->initValFromUserString(value);
}

/*! \brief Called when a Property added in AddProperty() has its run value chaged. Emits propertyValueChanged().
*/
void PropertyTable::propValueChange(Property* changedProp,QVariant)
{
	emit propertyValueChanged(changedProp);
}

/*! \brief Called when a Property added in AddProperty() has its init value chaged. Emits propertyInitValueChanged().
*/
void PropertyTable::propInitValueChange(Property* changedProp,QVariant)
{
	emit propertyInitValueChanged(changedProp);
}
