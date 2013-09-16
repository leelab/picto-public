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
//class PropertyContainer;
/*! \brief a base class for anything that needs to write itself out as XML
 *
 *	Since most stuff is stored as XML, this class is incredibly useful (and yet really 
 *	simple).  Any object that is going to store itself as XML needs to inheret from
 *	this class and implement the two virtual functions for serializing/deserializing
 *	itself.
 *
 *	The DataStore class actually has no real functionality (it's basically just an interface).
 *
 *	The object is responsible for serializing/desersialing a complete XML fragment.  For example
 *	a VisualElement object will need to generate the tags <VisualElement> </VisualElement>, and
 *	all of the content in between.  Likewise, when deserialize is called, the VisualElement 
 *	should expect to receive an XMLStreamWriter that has <VisualElement> as the current token, and
 *	then will return with the </VisualElement> tag as the current token.
 *
 *	NOTE: Many of the child classes were created for this object before I started.  As such, I am not
 *	always sure what their intended purpose was.  Also, I may not have used this base class enough.
 *	A number of the protocol commands/responses include XML data that is definide within the 
 *	command/response.  It may be worth going back and refactoring some of that code. -MattG Sept 2010
 *	
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
	QStringList getDefinedChildTags(){return factories_.keys();};
	QStringList getOrderedPropertyList(){return orderedPropList_;};
	QList<QSharedPointer<Asset>> getGeneratedChildren(QString tagName);
	QStringList getAssociateChildTags(QUuid associateId);
	QList<QSharedPointer<Asset>> getAssociateChildren(QUuid associateId, QString tagName);
	QList<QSharedPointer<Asset>> getAssociateDescendants(QUuid associateId);
	virtual QList<QUuid> getAttachedAssociateIds();
	virtual QString identifier(){if(myTagName_ == "") {Q_ASSERT(defaultTagName() != "Default"); return defaultTagName();} return myTagName_;};
	virtual QString friendlyTypeName(){return "DataStore";};
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
	void childAddedAfterDeserialize(QSharedPointer<Asset> newChild);
	void associateChildEdited();
public slots:
	void childEdited();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	//AutoSerialization Stuff---------------------------------
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
	//void AddDefinableProperty(
	//	int type,
	//	QString tagName, 
	//	QVariant defaultValue,
	//	int minNumOfThisType, 
	//	int maxNumOfThisType
	//	);
	void AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>(),
		int minNumOfThisType = 1, 
		int maxNumOfThisType = 1
		);
	//void AddDefinableObject(QString tagName, QSharedPointer<Asset> object);
	void AddDefinableObjectFactory(QString tagName, QSharedPointer<AssetFactory> factory);
	void AddChild(QString tagName, QSharedPointer<Asset> child);

	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
	bool needsUniqueChildNames(){return needsUniqueChildNames_;};
	//Call this function to set whether children that want unique names must have unique names.
	//Default if you don't call this function is true.
	void requireUniqueChildNames(bool require = true){needsUniqueChildNames_ = require;};

	QSharedPointer<PropertyContainer> propertyContainer_;
	//--------------------------------------------------------


private:
	
	//AutoSerialization Stuff---------------------------------
	QMap<QString,QList<QSharedPointer<Asset>>> children_;
	QMap<QUuid,QMap<QString,QList<QSharedPointer<Asset>>>> associateChildrenByGuid_;
	QMap<QString,QSharedPointer<AssetFactory>> factories_;
	QStringList orderedPropList_;
	QString tagText_;
	//--------------------------------------------------------

	QString myTagName_;
	bool needsUniqueChildNames_;
	int assetId_;

};

}; //namespace Picto

#endif
