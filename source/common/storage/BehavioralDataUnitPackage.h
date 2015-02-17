#ifndef _BEHAVIORALDATAUNITPACKAGE_H_
#define _BEHAVIORALDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "BehavioralDataUnit.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores behavioral data (data from a SignalChannel) as a list BehavioralDataUnits (ie. x,y readings)
 *
 *	\details Each Package includes an ActionFrame which is the frame after which the data was read.  It also
 *	inclues an OffsetTime which is the amount of time after the prior frame that the first sample was read on 
 *	the SignalChannel.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API BehavioralDataUnitPackage : public DataUnit
#else
class BehavioralDataUnitPackage : public DataUnit
#endif
{
public:

	BehavioralDataUnitPackage();
	virtual ~BehavioralDataUnitPackage() {};

	void setDescriptor(QString descriptor);
	/*! \brief Returns a comma separated list of value names for the input data.
	 * \details This string is used to identify the number of input values, and 
	 * is sent along with the data to describe individual data points significance.
	 * (ex. "xPos,yPos")
	 */
	QString getDescriptor(){return descriptor_;};
	void setChannel(QString channel);
	/*!	\brief Sets the offset from the time that the Action frame occured to the time that the first sample in thie package was read (as a double).*/
	void setOffsetTime(double t){time_ = QString::number(t,'e',14);};
	/*!	\brief Sets the offset from the time that the Action frame occured to the time that the first sample in thie package was read (as a string).*/
	void setOffsetTime(QString t){time_ = t;};
	/*!	\brief Returns the offset from the time that the Action frame occured to the time that the first sample in thie package was read.*/
	QString getOffsetTime(){return time_;};
	/*! \brief Sets the number of milliseconds per sample stored here.*/
	void setResolution(double r){resolution_ = r;};
	/*! \brief Returns the number of milliseconds per sample stored here.*/ 
	double getResolution(){return resolution_;};
	/*! \brief Sets the FrameId of the frame after which this object's BehavioralData occured.
	*/
	void setActionFrame(qulonglong frameId){actionFrame_ = frameId;};
	/*! \brief Returns the FrameId of the frame after which this object's BehavioralData occured.
	*/
	qulonglong getActionFrame(){return actionFrame_;};
	void addData(double x, double y);
	void addData(QMap<QString, QVector<double>> signalChannelData, double frameToSampleOffset);

	void emptyData() { data_.clear(); };
	/*! \brief Returns the number of BehavioralDataUnits contained in this object.*/
	int length() { return data_.length(); }
	/*! \brief Returns the name of the SignalChannel being handled by this BehavioralDataUnitPackage.*/
	QString getChannel(){return channel_;};
	/*! \brief Returns the first BehavioralDataUnit in this list and removes it from the list.*/
	QSharedPointer<BehavioralDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	/*! \brief Returns the first BehavioralDataUnit in this list.*/
	QSharedPointer<BehavioralDataUnit> peekFirstDataPoint() { return data_.first(); };
	/*! \brief Returns the latest BehavioralDataUnit in this list and removes it from the list.*/
	QSharedPointer<BehavioralDataUnit> takeLastDataPoint() { return data_.takeLast(); };
	void clearAllButLastDataPoints();

	QByteArray getDataAsByteArray();
	qulonglong getDataIDOfLastUnit();

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString channel_;	//The signal channel from which this data was drawn
	double resolution_;
	QString time_;
	qulonglong actionFrame_;
	QString descriptor_;
	QList<QSharedPointer<BehavioralDataUnit>> data_;
};


}; //namespace Picto

#endif
