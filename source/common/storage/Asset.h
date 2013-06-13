#ifndef _ASSET_H_
#define _ASSET_H_

#include <QSharedPointer>
#include <QMap>

#include "../common.h"
#include "DesignConfig.h"
#include "Serializable.h"
namespace Picto {
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

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;
	virtual bool validateTree();
	
	virtual void setDeleted();
	void setEdited(){emit edited();};
	bool wasEdited(){return edited_;};
	bool isNew(){return isNew_;};
	bool isDeleted(){return deleted_;};
	void setParentAsset(QSharedPointer<Asset> parent){parent_ = parent;};
	virtual QString getName(){return "";};
	virtual QString identifier(){return "";};
	virtual QString friendlyTypeName(){return "";};
	QSharedPointer<Asset> getParentAsset(){return QSharedPointer<Asset>(parent_);};
	void setSelfPtr(QSharedPointer<Asset> selfPtr){self_ = selfPtr;};
	void reinitialize();
	virtual QString getInfo(){return QString("<h3 style=\"color:red\">%1</h3>").arg(friendlyTypeName());};
	virtual QString getDescription(){return QString("<h3 style=\"color:red\">%1</h3>").arg(friendlyTypeName());};
	QString getPath();
	bool needsUniqueName(){return needsUniqueName_;};	//Indicates that this objects name must be
														//unique within its parent

	virtual	int getAssetId(){return 0;};
	virtual void setAssetId(int){};

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);
	QSharedPointer<DesignConfig> getDesignConfig(){return designConfig_;};

	virtual void upgradeVersion(QString deserializedVersion);
signals:
	void edited();
	void deleted();
	void assetIdEdited();

protected:
	QSharedPointer<Asset> selfPtr(){return QSharedPointer<Asset>(self_);};
	virtual void preSerialize(){};
	virtual void preDeserialize();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader){return true;};
	void addError(QString errorMessage);
	// In some cases, an asset may not want to be considered edited, even if it already has been, 
	// unless it gets changed after a certain point.
	void setUnedited(){edited_ = false;};

	//Call this function from a child class if no other element on this classes level may have the same name.
	void requireUniqueName(bool require = true){needsUniqueName_ = require;};
	
	QSharedPointer<DesignConfig> designConfig_;
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
