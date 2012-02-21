#ifndef _SIGNALCHANNELINFO_H_
#define _SIGNALCHANNELINFO_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores Information about a signal channel
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API SignalChannelInfo : public DataUnit
#else
class SignalChannelInfo : public DataUnit
#endif
{
public:
	SignalChannelInfo();
	SignalChannelInfo(	QString name,
						QString tableName_,
						int stateVariableId,
						double resolution,
						int subchannels,
						QString subchannelNames);
	
	QString getName(){return name_;};
	QString getTableName(){return tableName_;};
	int getStateVariableId(){return stateVariableId_;};
	double getResolution(){return resolution_;};
	int getSubchannels(){return subchannels_;};
	QString getSubchannelNames(){return subchannelNames_;};

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QString name_;
	QString tableName_;
	int stateVariableId_;
	double resolution_;
	int subchannels_;
	QString subchannelNames_;
};


}; //namespace Picto

#endif
