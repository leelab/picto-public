#ifndef _SERIALIZABLE_H
#define _SERIALIZABLE_H

#include "../common.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QStringList>

namespace Picto {
//class Serialiable;
/*! \brief An interface for all things serializable.
 *
 *	
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Serializable: public QObject
#else
class Serializable: public  QObject
#endif
{
	Q_OBJECT
public:
	Serializable();

	static QString getErrors();
	static void clearErrors() { errors_.clear(); };

	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	//AutoSerialization Stuff---------------------------------
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;
	virtual void setDeleted();
	bool wasEdited(){return edited_;};
	bool isNew(){return isNew_;};
	bool isDeleted(){return deleted_;};

signals:
	void edited();
	void deleted();
	//--------------------------------------------------------

protected:
	void addError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void addError(QString objectType, QString errorMsg);

private:
	
	//AutoSerialization Stuff---------------------------------
	void AddError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader, QSharedPointer<QStringList> errors);
	bool edited_;
	bool isNew_;
	bool deleted_;
	//--------------------------------------------------------

	static QStringList errors_;

private slots:
	void receivedEditedSignal();
	void receivedDeletedSignal();
};

}; //namespace Picto

#endif
