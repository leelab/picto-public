#ifndef _DATASOURCEINFO_H_
#define _DATASOURCEINFO_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores Information about a paricular data source (ie. lfp or spike)
 *	\details Stored data is the name of the data source, the name of the table to which its data should be
 *	saved in the Session file, and the resolution (really this is the sample period) of sampling on that
 *	data source.
 *	\note In the case of spikes, resolution is the time per sample for a spike's waveform.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	/*! \brief Returns the name of the DataSource (ie. "lfp" or "spikes").*/
	QString getName(){return name_;};
	/*! \brief Returns the name of the table to be associated with data source.*/
	QString getTableName(){return tableName_;};
	/*! \brief Returns the time between each sample in the source (for spikes this is in reference to the waveform).*/
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
