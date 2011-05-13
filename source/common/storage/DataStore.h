#ifndef _DATASTORE_H_
#define _DATASTORE_H_

#include "../common.h"
#include "DataStoreFactory.h"
//#include "../property/PropertyContainer.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QRect>
#include <QPoint>
#include <QColor>
#include <QString>
#include <QMap>
#include <QList>

namespace Picto {
class DataStoreFactory;
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
class PICTOLIB_API DataStore : public QObject
#else
class DataStore : public  QObject
#endif
{
	Q_OBJECT
public:
	DataStore();

	//virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool serializeDataID(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeDataID(QSharedPointer<QXmlStreamReader> xmlStreamReader);


	qulonglong getDataID();

	static QString getErrors();
	void clearErrors() { errors_.clear(); };


	//AutoSerialization Stuff---------------------------------
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void setDeleted();
	bool wasEdited(){return edited_;};
	bool isNew(){return isNew_;};
	bool isDeleted(){return deleted_;};

public slots:
	void childEdited();

signals:
	void edited();
	//--------------------------------------------------------

protected:
	void addError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void addError(QString objectType, QString errorMsg);

	//To make the serialization and desrialization routines easier, the following mini
	//functions are provided, for serializing and deserializing common data types.
	//Note that these don't cover all possible data types, so you should expect
	//to write some of your own code...
	void serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QPoint point);
	void serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QRect rect);
	void serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QColor color);

	QPoint deserializeQPoint(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QRect deserializeQRect(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QColor deserializeQColor(QSharedPointer<QXmlStreamReader> xmlStreamReader);



	//AutoSerialization Stuff---------------------------------
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader){return true;};
	void AddDataStoreFactory(QString name,QSharedPointer<DataStoreFactory> factory);
	void AddProperty(QSharedPointer<DataStore> prop);
	QList<QSharedPointer<DataStore>> getGeneratedDataStores(QString factoryName); 
	//QSharedPointer<PropertyContainer> propertyContainer_;
	//--------------------------------------------------------



private:
	
	//AutoSerialization Stuff---------------------------------
	void AddError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader, QSharedPointer<QStringList> errors);
	QMap<QString,QList<QSharedPointer<DataStore>>> children_;
	QMap<QString,QSharedPointer<DataStoreFactory>> factories_;
	QString tagText_;
	bool edited_;
	bool isNew_;
	bool deleted_;
	//--------------------------------------------------------



	static qulonglong generateDataID();
	static QStringList errors_;
	static qulonglong lastDataID_;
	qulonglong dataID_;
	QString myTagName_;

};

}; //namespace Picto

#endif
