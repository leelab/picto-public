#ifndef _ASSET_H_
#define _ASSET_H_

#include "../common.h"
#include "Serializable.h"

#include <QSharedPointer>

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

	using Serializable::toXml;
	using Serializable::fromXml;
	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader){return fromXml(xmlStreamReader,true);};
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader, bool validate);
	void initializeToDefault(QString tagName = "", QString type = "");

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader, bool validate) = 0;
	virtual bool validateTree();
	
	virtual void setDeleted();
	void setEdited(){emit edited();};
	bool wasEdited(){return edited_;};
	bool isNew(){return isNew_;};
	bool isDeleted(){return deleted_;};
	void setParentAsset(QSharedPointer<Asset> parent){parent_ = parent;};
	virtual QString getName(){return "";};
	virtual QString identifier(){return "";};
	virtual QString assetType(){return "";};
	QSharedPointer<Asset> getParentAsset(){return QSharedPointer<Asset>(parent_);};
	void setSelfPtr(QSharedPointer<Asset> selfPtr){self_ = selfPtr;};
	void reinitialize();
	virtual QString getInfo(){return QString("<h3 style=\"color:red\">%1</h3>").arg(assetType());};
signals:
	void edited();
	void deleted();

protected:
	QSharedPointer<Asset> selfPtr(){return QSharedPointer<Asset>(self_);};
	virtual void postSerialize(){};
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader){return true;};
	// In some cases, an asset may not want to be considered edited, even if it already has been, 
	// unless it gets changed after a certain point.
	void setUnedited(){edited_ = false;};
private:

	bool edited_;
	bool isNew_;
	bool deleted_;
	QWeakPointer<Asset> parent_;
	QWeakPointer<Asset> self_;

private slots:
	void receivedEditedSignal();
	void receivedDeletedSignal();


};

}; //namespace Picto

#endif
