#include <qDebug>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include "IntanAlignmentTool.h"
#include "../../common/memleakdetect.h"

IntanAlignmentTool::IntanAlignmentTool()
{ 
	alignmentMutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	resetValues();
}
IntanAlignmentTool::~IntanAlignmentTool()
{
}

/*! \brief Resets IntanAlignmentTool variables to their initial values.
 */
void IntanAlignmentTool::resetValues()
{
	//set up the sums
	sumXX_ = 0.0;
	sumYY_ = 0.0;
	sumXY_ = 0.0;
	sumX_ = 0.0;
	sumY_ = 0.0;
	n_ = 0;

	//Before initial alignment, all neural timestamps will be converted to -1 in behavioral timebase.
	coeff_.A = -1.0;
	coeff_.B = 0.0;  
	coeff_.corr = 0.0;

	trials_ = 0; 
}
/*! \brief Converts the passed in time from neural to behavioral time using the current best fit parameters.
*/
double IntanAlignmentTool::convertToBehavioralTimebase(double neuralTime)
{
	QMutexLocker locker(alignmentMutex_.data());
	return coeff_.A + coeff_.B*neuralTime;
}

/*! \brief Converts the passed in time from behavioral to neural time using the current best fit parameters.
*/
double IntanAlignmentTool::convertToNeuralTimebase(double behavioralTime)
{
	QMutexLocker locker(alignmentMutex_.data());
	return (coeff_.B)?(behavioralTime - coeff_.A)/coeff_.B:-1;
}

/*! \brief Converts the passed in time span from neural to behavioral time using the current best fit parameters.
 *	\details This is like convertToBehavioralTimebase() except that the coeff_.A offset is not used.
*/
double IntanAlignmentTool::convertSamplePeriodToBehavioralTimebase(double neuralSamplePeriod)
{
	QMutexLocker locker(alignmentMutex_.data());
	return coeff_.B*neuralSamplePeriod;
}

/*! \brief Converts the passed in time span from behavioral to neural time using the current best fit parameters.
 *	\details This is like convertToNeuralTimebase() except that the coeff_.A offset is not used.
*/
double IntanAlignmentTool::convertSamplePeriodToNeuralTimebase(double behavioralSamplePeriod)
{
	QMutexLocker locker(alignmentMutex_.data());
	return (coeff_.B)?behavioralSamplePeriod/coeff_.B:-1;
}

/*! \brief Returns the jitter between a particular behavioral and neural timestamp (in behavioral time).
 *	\details In a perfect system, after alignment, behavioral and neural timestamps would be exactly the same.
 *	The jitter is the offset of these times after alignment.  It results from the fact that the system is not perfect.
 *	The event code transmission from Director to Proxy is not instantaneous.  Timestamping on event receipt/creation
 *	is not instantaneous, etc.
 */
double IntanAlignmentTool::getJitter(double bAlignTimestamp, double nAlignTimestamp)
{
	return bAlignTimestamp - convertToBehavioralTimebase(nAlignTimestamp);
}

/*! \brief Returns the correlation coefficient based on the last call to one of the alignment functions.
 *	\details The correlation coefficient is a measure of the quality of our least sqaures timing alignment.
 *	The closer to one our correlation coefficient is, the better the alignment.
 */
double IntanAlignmentTool::getCorrelationCoefficient()
{
	QMutexLocker locker(alignmentMutex_.data());
	return coeff_.corr;
}

/*! \brief Returns the Offset of the neural time from the behavioral time (ie. When is behavioral time zero with respect to neural time zero).
 *	\details This is A of BehavTime = A + B*NeuralTime.
 */
double IntanAlignmentTool::getNeuralOffsetTime()
{
	QMutexLocker locker(alignmentMutex_.data());
	return coeff_.A;
}

/*! \brief Returns the neural temporal factor (ie. How much faster does behavioral time increase than neural time).
 *	\details This is B of BehavTime = A + B*NeuralTime.
 */
double IntanAlignmentTool::getNeuralTemporalFactor()
{
	QMutexLocker locker(alignmentMutex_.data());
	return coeff_.B;
}

/*!	\brief Updates the coefficients when a new set of matching timestamps is found.
 *
 *	\details The function updates the running sums, and then uses those to calculate the newest coefficients.
 *
 *	The math is based on:
 *	  http://mathworld.wolfram.com/LeastSquaresFitting.html
 *	
 *	\note The neural timebase is the x-axis, while the behavioral
 *	timebase is the y-axis
 */
void IntanAlignmentTool::updateCoefficients(double bAlignTimestamp,
									   double nAlignTimestamp)

{
	//update the sums
	n_++;
	sumXX_ += nAlignTimestamp*nAlignTimestamp;
	sumYY_ += bAlignTimestamp*bAlignTimestamp;
	sumXY_ += nAlignTimestamp*bAlignTimestamp;
	sumX_ += nAlignTimestamp;
	sumY_ += bAlignTimestamp;

	//calulate the new coefficients
		//SSxx = sum(x[i]^2) - n*mean(x)^2
		//SSyy = sum(y[i]^2) - n*mean(y)^2
		//SSxy = sum(x[i]y[i]) - n*mean(x)*mean(y)
		
		//B = SSxy/SSxx
		//A = mean(y) - B*mean(x)
		//corr = r^2 = SSxy^2/(SSxx*SSyy)

	double meanX = sumX_/n_;
	double meanY = sumY_/n_;

	double SSxx = sumXX_ - n_*meanX*meanX;
	double SSyy = sumYY_ - n_*meanY*meanY;
	double SSxy = sumXY_ - n_*meanX*meanY;
	if(SSxx == 0)
	{	// This means the variance of the neural timestamps is zero (ie. only one value was ever received.
		// If this happens, its either the first trial, or something has gone horribly wrong.  If its the 
		// first trial SSyy and SSxy will also be zero and Coeff_.B should be 1 (in the limit), with the resulting value
		// for coeff_.A.  coeff_.corr should be 1 (in the limit), so we just set all the SS values to 1 to make these things 
		// happen.
		Q_ASSERT_X(n_==1,"IntanAlignmentTool::updateCoefficients()","A single neural timestamp value is being used repeatedly in alignment calculations.");
		SSxx = SSyy = SSxy = 1;
	}

	QMutexLocker locker(alignmentMutex_.data());
	coeff_.B = SSxy/SSxx;
	coeff_.A = meanY - coeff_.B*meanX;
	// Only update correlation coefficient once n_ >= 2.  This is because n==1 doesn't provide any slope information for our linear fit, only constant offset info
	// we want that fact reflected in the output.
	if(n_ < 2)
		coeff_.corr = 0;
	else
		coeff_.corr = (SSxy*SSxy)/(SSxx*SSyy);
}
