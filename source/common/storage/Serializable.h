#ifndef _SERIALIZABLE_H
#define _SERIALIZABLE_H

#include "../common.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QStringList>

namespace Picto {
//class Serialiable;
/*! \brief An interface for all things Serializable.
 *
 *	
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Serializable
#else
class Serializable
#endif
{
public:
	Serializable();
	virtual ~Serializable(){};

	static QString getErrors();
	static void clearErrors() { errors_.clear(); };
	QString toXml();
	bool fromXml(QString xmlText);

	virtual bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

protected:
	void addError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void addError(QString objectType, QString errorMsg);

private:
	
	void AddError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader, QSharedPointer<QStringList> errors);
	static QStringList errors_;

};

}; //namespace Picto

#endif
