#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include <QtVariantProperty.h>
#include <QVariant>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>
#include <QSharedPointer>

#include "../common.h"
#include "../storage/Asset.h"

namespace Picto {
class PropertyContainer;
/*!	\brief Describes the property of a DataStore.
 *
 *	All DataStore's in Picto must be entirely defined by Properties.  They store all of 
 *	the relevant data to completely reproduce the DataStore given its type.  By conforming to
 *	this design principle we can store a Design file by serializing only its tree structure
 *	and Property values and we can save an Experimental Session's complete experimental state by saving all changes
 *	to its Property values.  
 *
 *	An example of typical Properties would be a circle's color, radius, location, visibility, etc.  
 *	Each Property contains a name, a variant value and may include attributes (eg. allowable
 *	names for an enum property).  Properties may be deserialized from a Picto
 *  XML design file using deserializeFromXml().  Their values may also be converted to
 *	and from string form using valToUserString() and valFromUserString().  Underlying
 *  each Property are 3 values.  The saveValue, the initValue and the runValue.  
 *	Properties also run in two modes, run mode and design mode.  In design mode, 
 *	all calls to setValue() write to all three values and calls to value() return
 *	the saveValue.  In run mode, calls to setValue() set the runValue and calls to
 *	value() return the runValue.  The initValue may also be set by using setInitValue()
 *	and returned from initValue().  By separating these three values in this way, we
 *  can act on the same Property while designing and running an experiment.  Changes to values during
 *	an experimental run leave the underlying savable design untouched.  Init values
 *  are also important to keep the Picto run model intact.  An Operator can change initValues
 *	during an experimental run, but they are only set to the underlying runValues on
 *	entering a level in the StateMachine where they are accessible (when their parent enters scope).  
 *	In some unusual cases (ie. The experiment properties of xOffset, yOffset, etc), run Properties should
 *	be settable by the Operator even while the StateMachine is at a level where they are 
 *	already in scope.  For these cases, use enbleInitRunValueSync() which causes 
 *	these values to be copied to one another every time they are written.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Property : public Asset
#else
class Property : public Asset
#endif
{
	Q_OBJECT
public:
	virtual ~Property();
	int type();
	QString getName();
	void setAttribute(QString _attributeName, QVariant _attributeValue);
	QVariant attributeValue(QString _attributeName) const;
	QStringList getAttributes(){return attributes_.keys();};

	QVariant initValue();
	QVariant value();

	virtual void enableRunMode(bool enable);
	void enableInitRunValueSync(bool enable);

	/*! \brief Set this property as changeable during runtime.
	 *	\details If this is used and this Property's parent is marked as UIEnabled, a widget will
	 *	appear for this Property at runtime that will allow its initValue to be changed.  This should
	 *	be used on any Properties of a DataStore which the Operator might logically want to change during
	 *	runtime.  A CircleGraphic's Color Property for example should be setRuntimeEditable().  A CircleGraphic's
	 *	Name Property should not be setRuntimeEditable().
	 */
	void setRuntimeEditable(bool enabled = true){runtimeEnabled_ = enabled;};
	/*! \brief Returns whether this Property is runtime enabled.
	 *	\details See setRuntimeEditable() for more details.
	 *	\note This should really be called isRuntimeEditable() for consistancy with setRuntimeEditable().
	 */
	bool isRuntimeEnabled(){return runtimeEnabled_;};

	/*! \brief Sets whether this Property will have an associated widget in GUI Property editors.  Default is true.
	 *	\note This is different from setRuntimeEditable().  If setVisible() sets the Property as invisible it will
	 *	not appear even in the Design window.  This is useful in cases where we need a Property in order to make sure
	 *	that a DataStore is fully defined by Properties, but the Designer should not be able to alter that Property.
	 *	In the case of DataStores containing lists for example, we can use a Property to store a QVariantList, but 
	 *	that Property should not be editable.
	 */
	void setVisible(bool visible){visible_ = visible;};
	/*! \brief Returns whether this Property will have an associated widget in GUI Property editors.  Default is true.
	 *	\details See setVisible() for more details.
	 */
	bool isVisible(){return visible_;};

	/*! \brief Sets whether this Property will be editable in GUI property editors or read-only.  Default is true (editable).
	 */
	void setGuiEditable(bool guiEditable){guiEditable_ = guiEditable;};
	/*! \brief Gets whether this Property will be editable in GUI property editors or read-only.  Default is true (editable).
	 */
	bool isGuiEditable(){return guiEditable_;};
	
	/*! \brief Set this to true if this Property is the child of an AssociateElement.  This is used with read-only monitoring
	 *	for checking whether non-experimental scripts attempt to write to experimental Properties at runtime.
	 */
	void setAssociateProperty(bool isAssociate){associateProperty_ = isAssociate;};

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QString initValToUserString();
	void initValFromUserString(QString userString);
	QString valToUserString();
	void valFromUserString(QString userString);

	virtual QString identifier(){return tagName_;};
	virtual QString friendlyTypeName(){return "Property";};

	virtual	int getAssetId();
	virtual void setAssetId(int id);

	/*! \brief Returns true if a Property encountered an Obsolete syntax when it was deserializing itself from
	 *	XML at some point since the last call to clearObsoleteSerialSyntax().  This is useful for checking whether
	 *	an automatic syntax upgrade will occur on the next serialization.
	 */
	static bool encounteredObsoleteSerialSyntax(){return hadObsoleteSerialSyntax_;};
	/*! \brief Resets the hadObsoleteSerialSyntax_ flag to false for a new deserialization
	 *	\sa encounteredObsoleteSerialSyntax()
	 */
	static void clearObsoleteSerialSyntax(){hadObsoleteSerialSyntax_ = false;};
	
	/*! \brief Start monitoring whether Property values are being changed.
	 *	\details This is useful when scripts should be functioning in a "read-only" mode.  By using this function
	 *	along with the valueWasChanged() and changedValueName(), functions, runtime checking can be done to
	 *	make sure that read-only scripts are not doing any illegal writing.
	 *	
	 *	The procedure is as follows:
	 *		- A Picto element should call startMonitoringForValueChange() just before running the read-only script.  
	 *		- After the script code is complete, the element should call valueWasChanged().
	 *		- If valueWasChanged() returned true, the element should throw a scripting runtime error.
	 *		- The first value to have been changed since startMonitoringForValueChange() was called is available from changedValueName().
	 *			This can be used to provide more detail in the scripting error message.
	 */
	static void startMonitoringForValueChange(){valueWasChanged_ = false;};
	/*! \brief Functions like startMonitoringForValueChange() except that this function is used to make sure that Analysis Scripts
	 *	change only Analysis and not Experimental Property values.
	 *	\details The functions that are used in this case are startMonitoringForExperimentalValueChange(), experimentalValueWasChanged() and
	 *	changedExperimentalValueName().
	 *	The procedure is the same as for startMonitoringForValueChange().
	 */
	static void startMonitoringForExperimentalValueChange(){expValueWasChanged_ = false;};
	/*! \brief Returns true if a Property value was changed since the last call to startMonitoringForValueChange().
	 *	See startMonitoringForValueChange() for more detail.
	 */
	static bool valueWasChanged(){return valueWasChanged_;};
	/*! \brief Returns true if an Experimental Property value was changed since the last call to startMonitoringForExperimentalValueChange().
	 *	See startMonitoringForExperimentalValueChange() for more detail.
	 */
	static bool experimentalValueWasChanged(){return expValueWasChanged_;};
	/*! \brief Returns the name of the first Property to have its value changed since the last call to startMonitoringForValueChange().
	 *	See startMonitoringForValueChange() for more detail.
	 */
	static QString changedValueName(){return changedValueName_;};
	/*! \brief Returns the name of the first Experimental Property to have its value changed since the last call to startMonitoringForExperimentalValueChange().
	 *	See startMonitoringForExperimentalValueChange() for more detail.
	 */
	static QString changedExperimentalValueName(){return changedExpValueName_;};

public slots:
	void setInitValue(QVariant _value);
	void setValue(QVariant _value);

signals:
	/*! \brief Emitted whenever this Property's initValue changes.*/
	void initValueChanged(Property* changedProp,QVariant newInitValue);
	/*! \brief Emitted whenever this Property's runValue changes.*/
	void valueChanged(Property* changedProp,QVariant newValue);

protected:
	Property(int type, QString name, QVariant value);

	virtual QString variantToString(QVariant value) const;
	virtual QVariant stringToVariant(QString string, QString& error) const;
	virtual QVariant attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const;

private:
	friend class PropertyContainer;
	QVariant saveValuePrivate();
	void setSaveValuePrivate(QVariant _value);

	QVariant initValuePrivate();
	void setInitValuePrivate(QVariant _value);

	QVariant runValuePrivate();
	void setRunValuePrivate(QVariant _value);

	void AddSerializationAttribute(QString name);
	void SetSerializationAttributeValue(QString name, QVariant _value);
	QVariant GetSerializationAttributeValue(QString name);
	QMap<QString,QVariant> serializationAttributes_;
	void addSubProperty(QSharedPointer<Property> prop);
	void setName(QString name);
	int index_;
	bool syncInitAndRunVals_;
	bool runMode_;
	bool unsavedValsInitialized_;
	QString tagName_;
	QString tagText_;
	QString typeVal_; // In cases where a Asset Factory used a type attribute to choose between types, a type that we don't use but need to write out would be in the tag.
	bool scriptEditable_;
	bool runtimeEnabled_;
	int assetId_;
	QVariant saveValue_;
	QVariant initValue_;
	QVariant runValue_;
	int type_;
	QString name_;
	QMap<QString,QVariant> attributes_;
	bool visible_;
	bool guiEditable_;
	bool serialSyntaxUpgraded_;
	bool associateProperty_;
	static bool hadObsoleteSerialSyntax_;
	static bool valueWasChanged_;
	static bool expValueWasChanged_;
	static QString changedValueName_;
	static QString changedExpValueName_;
};


}; //namespace Picto

#endif
