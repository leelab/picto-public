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

	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;
	
	virtual void setDeleted();
	bool wasEdited(){return edited_;};
	bool isNew(){return isNew_;};
	bool isDeleted(){return deleted_;};

signals:
	void edited();
	void deleted();


private:
	
	bool edited_;
	bool isNew_;
	bool deleted_;

private slots:
	void receivedEditedSignal();
	void receivedDeletedSignal();


};

}; //namespace Picto

#endif
