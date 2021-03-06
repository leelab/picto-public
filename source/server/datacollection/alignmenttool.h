#ifndef ALIGNMENTTOOL_H
#define ALIGNMENTTOOL_H

#include <QList>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QMutex>

/*!	\brief Calculates the alignment coefficients to move the neural timebase into the behavioral timebase.
 *
 *	\details There are two seperate timebases in Picto.  The behavioral timebase is used
 *	to stamp the events that are recorded/generated by PictoDirector, while the
 *	neural timebase is used by the neural recording device (Plexon, TDT, etc).
 *	This object looks at the event codes that are generated
 *	and recorded (almost) simultaneously in both timebases and generates a best fit which can then 
 *	be used to convert between timebases.
 *
 *	PictoDirector generates the alignment events and stamps them with its
 *	timebase.  At approximately the same time, it sends an alignment code to
 *	the neural recording device.  The alignment code is a 7-bit value that rolls-over when
 *	it reaches 127 (126,127,0,1,...).
 *
 *	The tool assumes a linear alignment such that BehavTime = A + B*NeuralTime.  These A
 *	and B values can be access from getNeuralOffsetTime() and getNeuralTemporalFactor().
 *	Alternatively, convenience functions convertToBehavioralTimebase() and 
 *	convertSamplePeriodToBehavioralTimebase() are provided as are their counterparts for
 *	converting the timestamps in the opposite direction.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class AlignmentTool
{
public:
	AlignmentTool();
	virtual ~AlignmentTool();

	void resetValues();
	double convertToBehavioralTimebase(double neuralTime);
	double convertToNeuralTimebase(double behavioralTime);
	double convertSamplePeriodToBehavioralTimebase(double neuralSamplePeriod);
	double convertSamplePeriodToNeuralTimebase(double behavioralSamplePeriod);
	double getJitter(double bAlignTimestamp, double nAlignTimestamp);
	double getCorrelationCoefficient();
	double getNeuralOffsetTime();
	double getNeuralTemporalFactor();
	void updateCoefficients(double bAlignTimestamp, double nAlignTimestamp);

private:
	struct AlignmentEvent
	{
		int id;
		int alignCode;
		int trialNum;
		double timestamp;
	};

	//Y = A + BX
	//Y - behavioral timebase
	//X - neural timebase
	//corr = correlation coefficient
	typedef struct 
	{
		double A;
		double B;
		double corr;
	} coefficients;

	coefficients coeff_;

	//Fitting values
	//SumXX = sum(x[i]^2)
	//SumYY = sum(y[i]^2)
	//SumXY = sum(x[i]y[i])
	//SumX = sum(x[i])  used to calculate mean(x)
	//SumY = sum(y[i])  used to calculate mean(y)
	//n = number of points in fit
	double sumXX_, sumYY_, sumXY_;
	double sumX_, sumY_;
	int n_;

	//statistics
	int trials_;
	//QMutex* databaseWriteMutex_;
	QSharedPointer<QMutex> alignmentMutex_;
};


#endif

