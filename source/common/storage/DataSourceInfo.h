#ifndef _DATASOURCEINFO_H_
#define _DATASOURCEINFO_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores Information about a signal channel
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API DataSourceInfo : public DataUnit
#else
class DataSourceInfo : public DataUnit
#endif
{
public:
	DataSourceInfo();
	DataSourceInfo(	QString name,
						QString tableName_,
						double resolution);
	
	QString getName(){return name_;};
	QString getTableName(){return tableName_;};
	double getResolution(){return resolution_;};

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QString name_;
	QString tableName_;
	double resolution_;
};


}; //namespace Picto

#endif
