#ifndef _SIGNALCHANNELINFO_H_
#define _SIGNALCHANNELINFO_H_
#include <QList>
#include <QMap>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores information about a SignalChannel 
 *
 *	\details Objects of this class store meta-data about a particular SignalChannel including its
 *	name, the session table that holds its data, the VariableId that identifies it in the session file, 
 *	its sample period, and the names of its subchannels.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Returns the SignalChannel name (ie. Position).*/
	QString getName(){return name_;};
	/*! \brief Returns the name of the Session table associated with the SignalChannel.*/
	QString getTableName(){return tableName_;};
	/*! \brief Returns the State Variable ID associated with the signal channel.*/
	int getStateVariableId(){return stateVariableId_;};
	/*! \brief Returns the number of seconds per sample in this SignalChannel.*/
	double getResolution(){return resolution_;};
	/*! \brief Returns the number of subchannels in this SignalChannel.*/
	int getSubchannels(){return subchannels_;};
	/*! \brief Returns a comma separated list of subchannel names for the signal channel. (ie. "xPos,yPos").
	 */
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
