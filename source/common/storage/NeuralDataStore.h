#ifndef _NEURALDATASTORE_H_
#define _NEURALDATASTORE_H_

#include "../common.h"
#include "DataStore.h"
#include <QString>

namespace Picto {

/*!	\brief Unimplemented. Neural Data.
 *
 *	I'm not sure exactly how this was going to be used, but it is clearly
 *	intended to store neural data.  I suspect this may already be covered by
 *	the ACQGetCommand Handler in the Proxy Server.  It would be smart to move
 *	the definition here, but I didn't have time to do so...
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API NeuralDataStore : public DataStore
#else
class NeuralDataStore : public DataStore
#endif
{
public:
	NeuralDataStore();
	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setTimestamp(double timestamp){timestamp_ = timestamp;}
	void setFittedtime(double fittedtime){fittedtime_ = fittedtime;}
	void setCorrelation(double correlation){correlation_ = correlation;}
	void setChannel(int channel){channel_ = channel;}
	void setUnit(int unit){unit_ = unit;}
	void setWaveform(QString waveform){waveform_ = waveform;}

	double getTimestamp(){return timestamp_;}
	double getFittedtime(){return fittedtime_;}
	double getCorrelation(){return correlation_;}
	int getChannel(){return channel_;}
	int getUnit(){return unit_;}
	QString getWaveform(){return waveform_;}

private:
double timestamp_;
double fittedtime_;
double correlation_;
int channel_;
int unit_;
QString waveform_;
};


}; //namespace Picto

#endif
