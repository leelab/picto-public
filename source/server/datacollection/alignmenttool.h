/*	\brief Aids in the alignment of the neural and behavioral timebases
 *
 *	There are two seperate timebases in Picto.  The behavioral timebase is used
 *	to stamp the events that are recorded/generated by PictoDirector, while the
 *	neural timebase is used by the neural recording device (Plexon, TDT, etc).
 *	This object looks at the trial start/end events where are generated
 *	simultaneously in both timebases and generates a best fit which can then 
 *	be used to convert between timebases.
 *
 *	PictoDirector generates trial start/end events and stamps them with its
 *	timebase.  At approximately the same time, it sends an alignment code to
 *	the neural recording device.  The alignment code is a 7-bit value that is
 *	the same for the start and end event.  The alignment codes roll-over when
 *	they reach 127 (126,127,0,1,...), but it is assumed that the events will
 *	more or less in order, so we don't worry about misaligning events.
 */

#ifndef ALIGNMENTTOOL_H
#define ALIGNMENTTOOL_H

#include <QList>
#include <QSqlDatabase>



class AlignmentTool
{
public:
	AlignmentTool(QSqlDatabase sessionDb);
	~AlignmentTool();

	double convertToBehavioralTimebase(double neuralTime);
	double convertToNeuralTimebase(double behavioralTime);

	void doFullAlignment();
	void doIncrementalAlignment();

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

	void updateCoefficients(AlignmentEvent bStartEvent,
							AlignmentEvent bEndEvent,
							AlignmentEvent nStartEvent,
							AlignmentEvent nEndEvent);

	QSqlDatabase sessionDb_;

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
};


#endif

