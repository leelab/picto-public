#ifndef _LFPDATAUNITPACKAGE_H_
#define _LFPDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include <QString>
#include <QVector>
#include <QList>
#include <QLinkedList>
#include <QMap>


namespace Picto {

/*!	\brief Stores local field data for transmission over a network.
 *
 *	\details Each LFPDataUnitPackage holds a list of LFP data that was read starting at
 *	a stored timestamp with a stored sample period for a particular stored channel.  The
 *	class also includes functions for setting "aligned" timestamps and sample periods so
 *	that the function can include lfp timing that is aligned to the behavioral timestream.
 *	The correlation coefficient for the least squares fitting of the timestamp alignment 
 *	can be stored here as well.
 *	\note Originally, we wanted to right LFP data on a row by row basis with one row for 
 *	each timestamp and columns for each channel, but with the current Sqlite database 
 *	this simply takes way too long.  We ran a benchmark test and found that
 *	each character stored in a new row takes an additional ~20us, whereas
 *	each character stored in the same row takes an additional ~20ns.
 *	Big difference.  For this reason, we are storing lfp data in ~500ms chunks
 *	indexed by timestamp and channel.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API LFPDataUnitPackage : public DataUnit
#else
class LFPDataUnitPackage : public DataUnit
#endif
{
public:
	LFPDataUnitPackage();
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	/*! \brief Sets the lfp channel whose data is stored in this object.
	 */
	void setChannel(int channel){channel_ = channel;};
	/*! \brief Sets the least squares correlation coefficient describing the quality of the neural to behavioral alignment
	 *	estimate.
	 */
	void setCorrelation(double correlation){correlation_ = correlation;}
	/*! \brief Sets the time at which the first LFP sample stored here occured.
	 *	\note Not yet aligned to behavioral timestream.
	 */
	void setTimestamp(double timestamp){timestamp_ = QString::number(timestamp,'e',14);};
	//! Sets the time at which the first LFP sample stored here occured as aligned to the behavioral timestream.
	void setFittedTimestamp(double timestamp){fittedTimestamp_ = QString::number(timestamp,'e',14);};
	//!	Sets the time per LFP sample in the neural timestream (not yet aligned to behavioral timestream).
	void setResolution(double resolution){resolution_ = resolution;};
	//!	Sets the time per LFP sample as aligned to the behavioral timestream.
	void setFittedResolution(double fittedresolution){fittedResolution_ = fittedresolution;};
	void appendData(const float* adPotentialReadings, int numVals);
	void appendData(float adPotentialReading);

	//!	Returns the number of LFP samples stored in this object.
	int numSamples() const { return numSamples_; };
	//! Returns the lfp channel whose data is stored in this object.
	int getChannel() const { return channel_; };
	/*! \brief Returns the least squares correlation coefficient describing the quality of the neural to behavioral
	 *	alignment estimate.
	 */
	double getCorrelation() const {return correlation_;}
	/*! \brief Returns the time at which the first LFP sample stored here occured.
	 *	\note Not yet aligned to behavioral timestream.
	 */
	QString getTimestamp() const {return timestamp_;};
	//! Returns the time at which the first LFP sample stored here occured as aligned to the behavioral timestream.
	QString getFittedTimestamp() const {return fittedTimestamp_;};
	/*! \brief Returns the time per LFP sample in the neural timestream.
	 *	\note Not yet aligned to behavioral timestream.
	 */
	double getResolution() const {return resolution_;};
	//!	Returns the time per LFP sample as aligned to the behavioral timestream.
	double getFittedResolution() const {return fittedResolution_;};
	//! Returns the list of Local Field Potentials as a single string with values separated by spaces.
	QString getPotentials() const {return potentials_.join(" ");};
	QByteArray getPotentialsAsByteArray() const;
	QLinkedList<QPointF> getAlignedDataAsLinkedList() const;
	void setPotentialsFromByteArray(QByteArray potentials);

private:
	int channel_;
	double correlation_;
	QString timestamp_;
	QString fittedTimestamp_;
	double resolution_;
	double fittedResolution_;
	QStringList potentials_;
	float* potArray_;
	int numSamples_;

};


}; //namespace Picto

#endif
