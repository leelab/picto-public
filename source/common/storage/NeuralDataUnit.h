#ifndef _NEURALDATAUNIT_H_
#define _NEURALDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"
#include <QString>
#include <QVector>

namespace Picto {

/*!	\brief Unimplemented. Neural Data.
 *
 *	I'm not sure exactly how this was going to be used, but it is clearly
 *	intended to store neural data.  I suspect this may already be covered by
 *	the ACQGetCommand Handler in the Proxy Server.  It would be smart to move
 *	the definition here, but I didn't have time to do so...
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API NeuralDataUnit : public DataUnit
#else
class NeuralDataUnit : public DataUnit
#endif
{
public:
	NeuralDataUnit();
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setTimestamp(double timestamp){timestamp_ = timestamp;}
	void setFittedtime(double fittedtime){fittedtime_ = fittedtime;}
	void setCorrelation(double correlation){correlation_ = correlation;}
	void setChannel(int channel){channel_ = channel;}
	void setUnit(int unit){unit_ = unit;}
	void setWaveform(QSharedPointer<QVector<float>> waveform);
	void setWaveformFromString(QString waveform);

	double getTimestamp(){return timestamp_;}
	double getFittedtime(){return fittedtime_;}
	double getCorrelation(){return correlation_;}
	int getChannel(){return channel_;}
	int getUnit(){return unit_;}
	QString getWaveformAsString();
	QByteArray getWaveformAsByteArray();
	void setWaveformFromByteArray(QByteArray waveform);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
double timestamp_;
double fittedtime_;
double correlation_;
int channel_;
int unit_;
QStringList waveform_;
};


}; //namespace Picto

#endif
