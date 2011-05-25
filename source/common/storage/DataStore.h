#ifndef _DATASTORE_H_
#define _DATASTORE_H_

#include "../common.h"
#include "Asset.h"
#include "AssetFactory.h"
#include "../property/PropertyContainer.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QRect>
#include <QPoint>
#include <QColor>
#include <QString>
#include <QMap>
#include <QList>

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

	//AutoSerialization Stuff---------------------------------
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void setDeleted();

public slots:
	void childEdited();

protected:

	////To make the serialization and desrialization routines easier, the following mini
	////functions are provided, for serializing and deserializing common data types.
	////Note that these don't cover all possible data types, so you should expect
	////to write some of your own code...
	//void serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
	//				QString name, QPoint point);
	//void serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
	//				QString name, QRect rect);
	//void serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
	//				QString name, QColor color);

	//QPoint deserializeQPoint(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	//QRect deserializeQRect(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	//QColor deserializeQColor(QSharedPointer<QXmlStreamReader> xmlStreamReader);



	//AutoSerialization Stuff---------------------------------
	virtual QString defaultTagName(){return "Default";};
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader){return true;};
	void initializePropertiesToDefaults();
	void AddDefinableProperty(
		QString tagName, 
		QVariant defaultValue, 
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>(),
		int minNumOfThisType = 1, 
		int maxNumOfThisType = 1
		);
	void AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QString singleAttributeName,
		QVariant singleAttributeValue,
		int minNumOfThisType = 1, 
		int maxNumOfThisType = 1
		);
	void AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue,
		int minNumOfThisType, 
		int maxNumOfThisType
		);
	void AddDefinableProperty(
		int type,
		QString tagName, 
		QVariant defaultValue, 
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>(),
		int minNumOfThisType = 1, 
		int maxNumOfThisType = 1
		);
	void AddDefinableObject(QString tagName, QSharedPointer<Asset> object);
	void AddDefinableObjectFactory(QString tagName, QSharedPointer<AssetFactory> factory);
	void DefinePlaceholderTag(QString tagName);

	QList<QSharedPointer<Asset>> getGeneratedChildren(QString tagName); 
	bool hasChildrenOfType(QString tagName);
	QSharedPointer<PropertyContainer> propertyContainer_;
	//--------------------------------------------------------



private:
	
	//AutoSerialization Stuff---------------------------------
	QMap<QString,QList<QSharedPointer<Asset>>> children_;
	QMap<QString,QSharedPointer<AssetFactory>> factories_;
	QString tagText_;
	//--------------------------------------------------------

	QString myTagName_;

};

}; //namespace Picto

#endif
