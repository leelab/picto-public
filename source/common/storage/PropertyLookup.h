//#ifndef _PROPERTYLOOKUP_H_
//#define _PROPERTYLOOKUP_H_
//
//#include "../common.h"
//#include "DataUnit.h"
//#include "../property/property.h"
//
//#include <QList>
//#include <QMap>
//
//namespace Picto {
//
///*!	\brief Stores a single unit of property data as a path, value, time triplet
// *
// *	The data in the PropertyLookup represents one property data transition.
// */
//
//#if defined WIN32 || defined WINCE
//	class PICTOLIB_API PropertyLookup : public DataUnit
//#else
//class PropertyLookup : public DataUnit
//#endif
//{
//public:
//	PropertyLookup();
//	
//	void clear(){lookupList_.clear();};
//	void addProperty(QSharedPointer<Property> prop);
//
//	//Data store functions
//	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
//	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
//	
//	struct PropInfo{int id; QString path;};
//	QList<PropInfo> lookupList_;
//
//protected:
//	void addProperty(int id,QString path);
//
//	virtual void postDeserialize();
//	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
//
//};
//
//
//}; //namespace Picto
//
//#endif
