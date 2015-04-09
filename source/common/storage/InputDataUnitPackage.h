#ifndef _INPUTDATAUNITPACKAGE_H_
#define _INPUTDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "InputDataUnit.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores input data (data from a SignalChannel) as a list of InputDataUnits (ie. x,y readings)
 *
 *	\details Each Package includes an ActionFrame which is the frame after which the data was read.  It also inclues an
 *	OffsetTime which is the amount of time after the prior frame that the first sample was read on the SignalChannel.
 *	\author Trevor stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API InputDataUnitPackage : public DataUnit
#else
	class InputDataUnitPackage : public DataUnit
#endif
{
public:
	/*! \brief Constructs an InputDataUnitPackage.*/
	InputDataUnitPackage();
	/*! \brief InputDataUnitPackage virtual destructor.*/
	virtual ~InputDataUnitPackage() {};

	void setDescriptor(QString descriptor);
	/*! \brief Returns a comma separated list of value names for the input data.
	 * \details This string is used to identify the number of input values, and is sent along with the data to describe
	 *	individual data points significance. (ex. "input0,input1,input2,input3,input4,input5,input6,input7")
	 */
	QString getDescriptor(){return descriptor_;};
	void setChannel(QString channel);
	/*!	\brief Sets the offset from the time that the Action frame occured to the time that the first sample in the
	 *	package was read (as a double).
	 */
	void setOffsetTime(double t){time_ = QString::number(t,'e',14);};
	/*!	\brief Sets the offset from the time that the Action frame occured to the time that the first sample in the
	 *	package was read (as a string).
	 */
	void setOffsetTime(QString t){time_ = t;};
	/*!	\brief Returns the offset from the time that the Action frame occured to the time that the first sample in the
	 *	package was read.
	 */
	QString getOffsetTime(){return time_;};
	/*! \brief Sets the number of milliseconds per sample stored here.
	 */
	void setResolution(double r){resolution_ = r;};
	/*! \brief Returns the number of milliseconds per sample stored here.
	 */ 
	double getResolution(){return resolution_;};
	/*! \brief Sets the FrameId of the frame after which this object's InputData occured.
	 */
	void setActionFrame(qulonglong frameId){actionFrame_ = frameId;};
	/*! \brief Returns the FrameId of the frame after which this object's InputData occured.
	 */
	qulonglong getActionFrame(){return actionFrame_;};
	void addData(double input[8]);
	void addData(QMap<QString, QVector<double>> signalChannelData, double frameToSampleOffset);

	/*! \brief Clears the current Data in this DataStore.
	 */
	void emptyData() { data_.clear(); };
	/*! \brief Returns the number of InputDataUnits contained in this object.
	 */
	int length() { return data_.length(); }
	/*! \brief Returns the name of the SignalChannel being handled by this InputDataUnitPackage.
	 */
	QString getChannel(){return channel_;};
	/*! \brief Returns the first InputDataUnits in this list and removes it from the list.
	 */
	QSharedPointer<InputDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	/*! \brief Returns the first InputDataUnits in this list.*/
	QSharedPointer<InputDataUnit> peekFirstDataPoint() { return data_.first(); };
	/*! \brief Returns the latest InputDataUnits in this list and removes it from the list.
	 */
	QSharedPointer<InputDataUnit> takeLastDataPoint() { return data_.takeLast(); };
	void clearAllButLastDataPoints();

	QByteArray getDataAsByteArray();
	qulonglong getDataIDOfLastUnit();

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	//! \brief Perhaps UNUSED.  MONITOR AND POSSIBLY DELETE.
	virtual void postDeserialize();
	//! \brief Perhaps UNUSED.  MONITOR AND POSSIBLY DELETE.
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	//! The signal channel from which this data was drawn
	QString channel_;
	//! The resolution of the signal channel
	double resolution_;
	//! The offset from the time that the Action frame occured to the time that the first sample in the package was read
	QString time_;
	//! The FrameId of the frame after which this object's InputData occured.
	qulonglong actionFrame_;
	//! A comma separated string of value names for the input data.
	QString descriptor_;
	//! A QList of InputDataUnits handled by this package.
	QList<QSharedPointer<InputDataUnit>> data_;
};


}; //namespace Picto

#endif
