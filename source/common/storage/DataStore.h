#ifndef _DATASTORE_H_
#define _DATASTORE_H_

#include "../common.h"
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QRect>
#include <QPoint>
#include <QColor>
#include <QString>
#include <QMap>
#include <QList>
#include <QUuid>

#include "Asset.h"
#include "AssetFactory.h"
#include "../property/PropertyContainer.h"
#include "../storage/SearchRequest.h"

namespace Picto {
class AssetFactory;

/*! \brief A base class for all Asset objects that can have children
 *	\details The DataStore is a powerful and vitally important class for the Picto experimental design system.
 *	It provides the framework for parent-child relationships of all objects in the Design that can have children.
 *	The DataStore also includes the Property Containment framework that is at the heart of Picto's data storage
 *	and transfer system.  Every DataStore must be entirely defined by Property objects.  In otherwords, by looking
 *	at the values of a DataStore descendant's Properties, we should always be able to determine exactly what is 
 *	going on in a DataStore without needing access to any of the object's other member variables.  If this rule
 *	is followed, then all we need to do to serialize out any DataStore is to serialize out its type, its children 
 *	and its Property values.  Similarly, if we want to store the current state of any DataStore descendant, all we
 *	need to know are its type and Property values.  The Picto Experimental Execution system makes use of this 
 *	fact.  Apart from the various input signals and timing data, the Director needs to send only behavioral inputs 
 *	Transition traversals and Property value changes to the Picto Server in order to give it a complete picture of 
 *	what is happening in the experiment, and the RemoteViewer and ReplayViewer need only look at these transition
 *	traversals and timing values, and property changes to get a complete picture of the experiment at any given
 *	time and provide a framework for accurate analysis.
 *	
 *	Beyond the Property and parent-child frameworks, the DataStore also provides a means of attaching
 *	AssociateElement children to itself.  AssociateElements are objects that are linked to an element that is part of the
 *	Picto experiment design but are not necessary for it to actually run.  Currently, these include UIData objects, which
 *	provides information about the positioning of elements' graphical representations in the Designer window and various
 *	Analysis elements (AnalysisVariable, AnalysisScriptHolder, etc) objects that are updated during the course of Session
 *	Database playback to perform analyses.  AssociateElement objects are saved in a separate XML tree from the
 *	Experimental Design but still need to be attached to these objects when they are Deserialized so that the experimental
 *	system can property update them.  This is handled in the DataStore.
 *
 *	Lastly, the DataStore provides a search interface that allows users to search DataStores in the Designer for strings.
 *	Not every field in a DataStore can or should be searched, but the framework that allows descendant classes to define
 *	what is searched as well as the interface to perform the search are handled by the DataStore.
 *
 *	Edit the DataStore class with great care.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API DataStore : public Asset
#else
class DataStore : public  Asset
#endif
{
	Q_OBJECT

public:
	DataStore();
	virtual ~DataStore();

	//AutoSerialization Stuff---------------------------------
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool validateTree();
	virtual void setDeleted();
	/*! \brief Returns a QStringList off all XML child tags that are allowed inside the XML tag for this DataStore
	 *	\details Since the DataStore stores child Asset objects according to tagName, this function can be used
	 *	with getGeneratedChildren() to loop through all of this DataStores child Assets.
	 */
	QStringList getDefinedChildTags(){return factories_.keys();};
	/*! \brief Returns a list of XML tag names for child Property objects ordered in the order that they should appear in
	 *	the user interface.
	 *	\note The list returned by this function will be a differently ordered subset of the result of
	 *	getDefinedChildTags().
	 */
	QStringList getOrderedPropertyList(){return orderedPropList_;};
	QList<QSharedPointer<Asset>> getGeneratedChildren(QString tagName);
	QStringList getAssociateChildTags(QUuid associateId);
	QList<QSharedPointer<Asset>> getAssociateChildren(QUuid associateId, QString tagName);
	QList<QSharedPointer<Asset>> getAssociateDescendants(QUuid associateId);
	virtual QList<QUuid> getAttachedAssociateIds();
	virtual QString identifier(){if(myTagName_ == "") {Q_ASSERT(defaultTagName() != "Default"); return defaultTagName();} return myTagName_;};
	virtual QString friendlyTypeName(){return "DataStore";};
	/*! \brief Returns the PropertyContainer object that stores this DataStore's properties.
	 */
	QSharedPointer<PropertyContainer> getPropertyContainer(){return propertyContainer_;};
	QStringList getValidChildTags();
	QSharedPointer<AssetFactory> getAssetFactory(QString tagName);
	QSharedPointer<Asset> createChildAsset(QString tagName,QString type,QString& error);
	QSharedPointer<Asset> importChildAsset(QString childXml,QString& error);
	void clear();
	virtual void setPropertyRuntimeEditable(QString propName, bool editable = true);
	QList<QSharedPointer<DataStore>> getRuntimeEditableDescendants();
	void enableRunModeForDescendants(bool en);
	QList<QSharedPointer<Property>> getDescendantsProperties();
	/*! \brief Returns true if this DataStore exposes properties whose InitValues should be editable during the course of
	 *	a running experiment.
	 *	\details During the course of a running experiment, any DataStore can set some of its properties to be editable.
	 *	This means that when the operator changes their values, the change will be stored in an InitValue field inside
	 *	the property.  When the DataStore next enters scope, all of the Property runValues are reset to their initValues
	 *	such that the operator's changes take effect.  This function tells the RemoteViewer which DataStore objects
	 *	should have their properties exposed in the UI so that the operator will have the option to change them during
	 *	the course of the experiment.
	 *	\sa Property
	 */
	virtual bool isRuntimeEditable(){return false;};

	virtual	int getAssetId();
	virtual void setAssetId(int id);

	virtual void upgradeVersion(QString deserializedVersion);

	virtual bool isPartOfSearch(SearchRequest searchRequest);
	virtual bool searchForQuery(SearchRequest searchRequest);
	bool searchRecursivelyForQuery(SearchRequest searchRequest);
	bool searchChildrenForQuery(SearchRequest searchRequest);
	bool searchChildrenRecursivelyForQuery(SearchRequest searchRequest);
	bool searchParentForQuery(SearchRequest searchRequest);
	bool searchAncestorsForQuery(SearchRequest searchRequest);

	bool AddAssociateChild(QUuid associateId, int parentId, QString tagName, QSharedPointer<Asset> child);
	bool AddAssociateChild(QUuid associateId, QString parentPath, QString tagName, QSharedPointer<Asset> child);
	virtual void AddAssociateChild(QUuid associateId, QString tagName, QSharedPointer<Asset> child);
	void ClearAssociateDescendants(QUuid associateId);
	void ClearAllAssociateDescendants();
	virtual void ClearAssociateChildren(QUuid associateId);
	void ClearAllAssociateChildren();
	
	//Used as part of runtime checking for non-experiment scripts changing experimental properties
	//This can be called by any AssociateElement to set an "Associate" flag on its descendant
	//properties.  Properties without this flag will then report if they have been changed
	//when they shouldn't have been.
	void setDescendantPropertiesAsAssociates();

signals:
	/*! \brief Emitted whenever a child is added to this DataStore after its Deserialize function has already run.
	 *	\details The function is emitted when either Experimental or Associate children are added.
	 */
	void childAddedAfterDeserialize(QSharedPointer<Asset> newChild);
	/*! \brief Emitted whenever one of this DataStore's Associate children is edited.
	 */
	void associateChildEdited();
public slots:
	void childEdited();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	//AutoSerialization Stuff---------------------------------

	/*! \brief Returns the name of this DataStore's default XML tag.
	 *	\note The one who determines a child's tag name is its parent however, since it is its parent that needs to
	 *	understand which XML tags need to be deserialized in which types of Asset object.  See the
	 *	OutputElementContainer::OutputElementContainer() constructor for an example of this.
	 */
	virtual QString defaultTagName(){return "Default";};
	void initializePropertiesToDefaults();
	void AddDefinableProperty(
		QString tagName, 
		QVariant defaultValue
		);
	void AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QString singleAttributeName,
		QVariant singleAttributeValue
		);
	void AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>(),
		int minNumOfThisType = 1, 
		int maxNumOfThisType = 1
		);
	void AddDefinableObjectFactory(QString tagName, QSharedPointer<AssetFactory> factory);
	void AddChild(QString tagName, QSharedPointer<Asset> child);

	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
	/*! \brief Returns true if all children whose needsUniqueName() function returns true must in fact have unique names
	 *	for syntax to be considered valid.
	 */
	bool needsUniqueChildNames(){return needsUniqueChildNames_;};
	/*! \brief Call this function to set whether children that want unique names (ie. needsUniqueName() == true) must in
	 *	fact have unique names for syntax to be considered valid.
	 *	\details The default if you don't call this function is true, and needsUniqueName() children must have unique
	 *	names.
	 */
	void requireUniqueChildNames(bool require = true){needsUniqueChildNames_ = require;};

	//! The property container managing this DataStore's properties.
	QSharedPointer<PropertyContainer> propertyContainer_;

	//--------------------------------------------------------


private:
	
	//AutoSerialization Stuff---------------------------------
	//! Stores lists of child Assets according to their XML tags
	QMap<QString,QList<QSharedPointer<Asset>>> children_;
	//! Stores XML tag based lookup tables of associate Children by their Associate ID GUID.
	QMap<QUuid,QMap<QString,QList<QSharedPointer<Asset>>>> associateChildrenByGuid_;
	//! Stores AssetFactories based on the XML tag of the Asset type that they create
	QMap<QString,QSharedPointer<AssetFactory>> factories_;
	//! Stores the XML tags of child properties in the order that they should appear in the UI
	QStringList orderedPropList_;
	/*!	\brief Stores the XML tag contents which were deserialized into this DataStore.  Only child tags without their
	 *	contents are stored here.
	 */
	QString tagText_;										
	//--------------------------------------------------------

	//! Stores the XML tag under which this DataStore was deserialized.
	QString myTagName_;
	/*! \brief Stores whether children of this DataStore that want their names to be unique, must indeed have unique
	 *	names for design syntax to be valid.
	 */
	bool needsUniqueChildNames_;
	//! Stores this DataStore's Asset ID
	int assetId_;
};

}; //namespace Picto

#endif
