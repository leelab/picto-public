#ifndef _NEURALDATAUNIT_H_
#define _NEURALDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"
#include <QString>
#include <QVector>

namespace Picto {

/*!	\brief Stores Neural Spike data for transmission over a network.
 *
 *	\details Each NeuralDataUnit holds the time, channel, unit and waveform of a neural spike along with the
 *	sample period of the waveform data.  The class also includes functions for setting the "aligned" spike 
 *	timestamp and waveform sample periods so that the function can include spike timing that is aligned to the 
 *	behavioral timestream.  The correlation coefficient for the least squares fitting of the timestamp alignment 
 *	can be stored here as well.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

	/*! \brief Sets the time at which the spike occured (not yet aligned to behavioral timestream).*/
	void setTimestamp(double timestamp){timestamp_ = timestamp;}
	/*! \brief Sets the time at which the spike occured as aligned to the behavioral timestream.*/
	void setFittedtime(double fittedtime){fittedtime_ = fittedtime;}
	/*! \brief Sets the least squares correlation coefficient describing the quality of the neural to behavioral alignment
	 *	estimate.
	 */
	void setCorrelation(double correlation){correlation_ = correlation;}
	/*! \brief Sets the channel on which the spike occured.*/
	void setChannel(int channel){channel_ = channel;}
	/*! \brief Sets the unit on which the spike occured.*/
	void setUnit(int unit){unit_ = unit;}
	/*! \brief Sets the time per spike waveform sample.*/
	void setResolution(double secPerSamp){resolution_ = secPerSamp;};
	void setWaveform(QSharedPointer<QVector<float>> waveform);
	void setWaveformFromString(QString waveform);

	/*! \brief Returns the time at which the spike occured (not yet aligned to behavioral timestream).*/
	double getTimestamp() const {return timestamp_;}
	/*! \brief Returns the time at which the spike occured as aligned to the behavioral timestream.*/
	double getFittedtime() const {return fittedtime_;}
	/*! \brief Returns the least squares correlation coefficient describing the quality of the neural to behavioral alignment
	 *	estimate.
	 */
	double getCorrelation() const {return correlation_;}
	/*! \brief Returns the channel on which the spike occured.*/
	int getChannel() const {return channel_;}
	/*! \brief Returns the unit on which the spike occured.*/
	int getUnit() const {return unit_;}
	/*! \brief Returns the time per spike waveform sample.*/
	double getResolution() const {return resolution_;};
	QString getWaveformAsString() const;
	QByteArray getWaveformAsByteArray() const;
	void setWaveformFromByteArray(QByteArray waveform);

private:
	double timestamp_;
	double fittedtime_;
	double correlation_;
	double resolution_;
	int channel_;
	int unit_;
	QStringList waveform_;
};


}; //namespace Picto

#endif
