#ifndef _ASSET_H_
#define _ASSET_H_

#include <QSharedPointer>
#include <QMap>

#include "../common.h"
#include "DesignConfig.h"
#include "TaskConfig.h"
#include "Serializable.h"
namespace Picto {
/*! \brief The base class for all elements of a Picto experiment/analysis design
 *	\details Every component in a Picto design is an asset.  The entire design needs to be serializable, which is why
 *	Asset extends Serializable.  We also need to know when an asset is changed or deleted.  We need to be able to check
 *	the validity of its data.  We need to know where it lies in the Design tree, how to load it from a deprecated XML
 *	design, and we need to be able to access it quickly from a Hash table if need be, meaning that it has to have a
 *	unique ID.  The Asset class adds all of this functionality and more.  It is of key importance in the experimental
 *	design system and to Picto as a whole.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Asset : public QObject, public Serializable
#else
class Asset : public QObject, public Serializable
#endif
{
	Q_OBJECT
public:
	Asset();
	virtual ~Asset(){};

	using Serializable::toXml;
	using Serializable::fromXml;
	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void initializeToDefault(QString tagName = "", QString type = "");
	bool initializeFromXml(QString xml);
	virtual void enableRunMode(bool enable);
	/*! \brief Saves this object to descriptive XML text using the input QXmlStringWriter.
	 *	\details Returns true if serialization was successful, false otherwise.
	 */
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	/*! \brief Loads this object from a QXmlStringReader containing descriptive XML text with the read pointer located at
	 *	this Asset's start tag.
	 *	\details Returns true if serialization was successful, false otherwise
	 */
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;
	virtual bool validateTree();
	
	virtual void setDeleted();
	/*! \brief Emits the edited() signal when called to inform interested parties that something about this asset has
	 *	changed.
	 *	\details Since internally we connect edited() to receivedEditedSignal(), it also causes our edited_ flag to be
	 *	set so that this object itself will know if it has been edited.
	 */
	void setEdited(){emit edited();};
	/*! \brief Returns true if this object was edited since it was deserialized.
	 *	\details Deserialization obviously changes the asset as well; however, we reset the underlying edited_ flag back
	 *	to false when deserialization completes so that we can tell if the asset is the same or different from the one
	 *	described by the input descriptive xml.
	 */
	bool wasEdited(){return edited_;};
	/*! \brief Returns true if this asset was created from scratch and was not configured according XML code from some
	 *	previously edited asset.
	 *	\details An asset may be both new and edited if it was created in the designer and then configured.  Having
	 *	separate new and edited flags is useful in order to limit the size of serialized data and also for the purpose of
	 *	maintaining a general order and structure in xml design files from one serialization to the next to aid in
	 *	comparison of different versions of a design file.
	 *	\sa DataStore::serializeAsXml(), DataStore::deserializeFromXml()
	 */
	bool isNew(){return isNew_;};
	/*! \brief Returns true if this asset was set as deleted.
	 *	\details Currently in Picto, in order to delete an asset we just set it as deleted, then when the time comes for
	 *	serialization, we don't serialize out any of the "delete marked" assets.  Doing things this way helps us maintain
	 *	a general order and structure in xml design files from one serialization to the next to aid in comparison of
	 *	different versions of a design file.
	 * \sa DataStore::serializeAsXml(), DataStore::deserializeFromXml()
	 */
	bool isDeleted(){return deleted_;};
	/*! \brief Assets are meant to be part of a tree structure.  This function sets this Asset object's parent to the
	 *	input.
	 *	\details The parent is stored locally as a weak pointer to avoid circular smart pointer references between parent
	 *	and child.  In the Picto Design, parents maintain smart pointers to children, such that deleting the top level
	 *	node has the effect of deleting the entire tree underneath it.
	 */
	void setParentAsset(QSharedPointer<Asset> parent){parent_ = parent;};
	/*! \brief Returns the name of this asset.
	 */
	virtual QString getName(){return "";};
	/*! \brief Returns a short string used to identify this type of Asset.
	 *	\details Usually this is the same as this assets serialized XML tag name, but it may not be.
	 */
	virtual QString identifier(){return "";};
	/*! \brief Returns a friendly string identifying this type of Asset.
	 *	\details This is a string that users will see, so check the spelling... we've had problems with that in the past
	 *	(ie. TargetController, "Total Time Excceeded" result).
	 */
	virtual QString friendlyTypeName(){return "";};
	/*! \brief Returns a pointer to this Asset object's parent Asset.
	 *	\details If the parent was never set, was already deleted, or doesn't exist, an empty QSharedPointer will be
	 *	returned.
	 */
	QSharedPointer<Asset> getParentAsset(){return QSharedPointer<Asset>(parent_);};
	/*! \brief Sets a QSharedPointer to this Asset to this Asset so that it will be able to give others QSharedPointers to
	 *	itself, and not just regular pointers
	 *	\details Locally, the self pointer is stored as a QWeakPointer (otherwise the object could never be deleted).
	 */
	void setSelfPtr(QSharedPointer<Asset> selfPtr){self_ = selfPtr;};

	/*! \brief Returns information about this object for the user.  In practice, the AssetDescriber has taken over this
	 *	functionality to a great extent.
	 *	\details Originally, we planned to use this to store information describing an Asset inside its class description.
	 *	We are still using it to automatically generate script information in the Picto Designer's "Scripting Info" tab
	 *	which shows all available in scope elements along with their scripts, but this tab has not been well maintained
	 *	and should either be updated or removed since scripting documentation is clearly available in the "About Element"
	 *	box which gathers much clearer and fully documented information from the AssetDescriber.  We should consider
	 *	removing that tab and this function.
	 */
	virtual QString getInfo(){return QString("<h3 style=\"color:blue\">%1</h3>").arg(friendlyTypeName());};
	QString getPath();
	/*! \brief Returns true if this object's name is required to be unique within its parent.
	 *	\details If an object is the parent of more than one asset with the same name and at least one of them has this
	 *	flag set, the parent should report a syntax violation.
	 *	\sa requireUniqueName()
	 */
	bool needsUniqueName(){return needsUniqueName_;};
	
	/*! \brief Return's this Asset's unique Asset ID.
	 *	\details Every Asset has a unique Asset ID.  This is used wherever Asset objects are entered into Hash
	 *	tables for quick access.  It is also very useful in the Session Databases as a means to store information
	 *	about assets and how their data changes without constantly referencing the Asset object's path.  Asset ID's
	 *	should be serialized out and deserialized in to assure for example that Data stored in a Session Database
	 *	referencing a particular asset id, can be correctly connected with a loaded Asset in the Workstation Playback
	 *	viewer.  Descendant classes that implement serialization should also implement setting of this assetID.
	 *	Original asset ID generation when an Asset is first generated; however, is handled by the DesignConfig class 
	 *	since it tracks all Assets in a Design and is unique over the whole design.
	 *	\sa setAssetId(), DesignConfig::disallowIdDuplication(), DesignConfig::fixDuplicatedAssetIds(), DesignConfig::addManagedAsset()
	 */
	virtual	int getAssetId(){return 0;};
	/*! \brief Sets this asset's asset ID. For more detail, see getAssetId().
	 *	\note I'm not sure why we didn't make this a pure virtual function... but we probably should.  This should be
	 *	looked into.
	 */
	virtual void setAssetId(int){};

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);
	/*! \brief Returns a QSharedPointer to the DesignConfig managing this asset.
	 *	\details Every Design has a single DesignConfig object that manages global issues affecting the entire design.
	 *	It can access every asset in the design and is accessible from every asset in the design.  It is set to an asset
	 *	using the setDesignConfig function.
	 *	\sa setDesignConfig, DesignConfig
	 */
	QSharedPointer<DesignConfig> getDesignConfig(){return designConfig_;};

	virtual void setTaskConfig(QSharedPointer<TaskConfig> taskConfig);
	/*! \brief Returns a QSharedPointer to the TaskConfig managing this asset.
	*	\details Every Design has one TaskConfig object for each task.  The TaskConfig object manages task-global issues.
	*	It is accessible from every asset in the design.  It is set to an asset using the setTaskConfig function.
	*	\sa setTaskConfig, TaskConfig
	*/
	QSharedPointer<TaskConfig> getTaskConfig(){ return taskConfig_; };

	virtual void upgradeVersion(QString deserializedVersion);
signals:
	/*! \brief Emitted whenever something in this asset changes
	 */
	void edited();
	/*! \brief Emitted when this Asset is marked to be deleted
	 *	\sa setDeleted(), isDeleted()
	 */
	void deleted();
	/*! \brief Emitted whenever this Asset's Asset ID changes
	 */
	void assetIdEdited();

protected:
	/*! \brief Returns a QSharedPointer to this Asset.
	 *	\details If this returns an empty QSharedPointer, it means that setSelfPtr() was never called and this Asset's
	 *	self pointer was never set.
	 */
	QSharedPointer<Asset> selfPtr(){return QSharedPointer<Asset>(self_);};
	/*! \brief This function is called before an asset is serialized in toXML().  
	 *	\details This is a convenience function that is called before serialization and should be used for any set up
	 *	needs to occur before serialization starts.
	 */
	virtual void preSerialize(){};
	virtual void preDeserialize();
	virtual void postDeserialize();
	/*! \brief Returns true if this Asset has valid syntax.  
	 *	\details If invalid syntax is found, this function uses addError to report a syntax violation and returns
	 *	false.  The QXmlStreamReader is passed in so that the Asset can reference it in any error
	 *	messages.
	 */
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader){return true;};
	void addError(QString errorMessage);
	/*! \brief In some cases, an asset may not want to be considered edited, even if it already has been, 
	 *	unless it gets changed after a certain point.  This allows for that.
	 *	\note Currently the only usage of this is in creating default results that hang off of various elements
	 *	and need a name but don't need to be serialized out.
	 */
	void setUnedited(){edited_ = false;};

	/*! \brief Set this to true if this object's name is required to be unique within its parent or false otherwise.
	 *	\details If an object is the parent of more than one asset with the same name and at least one of them
	 *	has this flag set, the parent should report a syntax violation.  By default, Asset's don't require unique names.
	 *	\sa needsUniqueName()
	 */
	void requireUniqueName(bool require = true){needsUniqueName_ = require;};

	//! \brief The design config managing this Asset and the Design containing it.
	QSharedPointer<DesignConfig> designConfig_;

	//! \brief The task config managing this Asset and the Task containing it.
	QSharedPointer<TaskConfig> taskConfig_;
private:

	bool edited_;
	bool isNew_;
	bool deleted_;
	QWeakPointer<Asset> parent_;
	QWeakPointer<Asset> self_;
	int assetId_;
	bool hasAssetId_;
	bool needsUniqueName_;

private slots:
	void receivedEditedSignal();
	void receivedDeletedSignal();


};

}; //namespace Picto

#endif
