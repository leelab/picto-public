#include <qDebug>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>

#include "alignmenttool.h"

AlignmentTool::AlignmentTool(QSqlDatabase sessionDb)
	: sessionDb_(sessionDb)
{
	Q_ASSERT(false);  //We need to relocate all of the database accesses to SessionInfo
	//set up the sums
	sumXX_ = 0.0;
	sumYY_ = 0.0;
	sumXY_ = 0.0;
	sumX_ = 0.0;
	sumY_ = 0.0;
	n_ = 0;

	//set up an initial fit of y=x with no correlation
	coeff_.A = 0.0;
	coeff_.B = 1.0;  
	coeff_.corr = 0.0;

	trials_ = 0; 

	qDebug()<<"!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!";
	qDebug()<<"The alignment tool has not been tested (due to the difficulty of creating a test program)";
	qDebug()<<"Do not use this code until it has been tested";

}
AlignmentTool::~AlignmentTool()
{
}

//!	\brief converts the passed in time from neural to behavioral time using the current best fit
double AlignmentTool::convertToBehavioralTimebase(double neuralTime)
{
	return coeff_.A + coeff_.B*neuralTime;
}

//!	\brief converts the passed in time from behavioral to neural time using the current best fit
double AlignmentTool::convertToNeuralTimebase(double behavioralTime)
{
	return (behavioralTime - coeff_.A)/coeff_.B;
}

/*! \brief Does a complete alignment on the session database
 *
 *	A complete alignment involves running through all of the trial start/stop events
 *	and calculating the coefficients.  This is time consuming, and probably shouldn't 
 *	be done while an experiment is running.  Note also that this resets the matched
 *	column in the trial tables.
 */
void AlignmentTool::doFullAlignment()
{
	QSqlQuery query(sessionDb_);

	//Clear out the trials table
	query.exec("DELETE FROM trials");

	//Reset the matched values to 0 (since we're going to rematch all of the events)
	query.exec("UPDATE neuraltrials SET matched = 0");
	query.exec("UPDATE behavioraltrials SET matched = 0");

	//Now that everything is unmatched, we can simply call the incremental
	//alignment function
	doIncrementalAlignment();

	QSqlQuery trialsQuery(sessionDb_);


	//since we have done a complete fit, we'll need to recalculate the
	//jitter value for each row in the trials table.
	trialsQuery.exec("SELECT id,neuralstart,neuralend,behavioralstart,behavioralend "
					 "FROM trials");

	while(trialsQuery.next())
	{
		QSqlQuery trialsInsertQuery(sessionDb_);

		double nStart, nEnd, bStart, bEnd;
		double startjitter,endjitter;
		int id;

		id=trialsQuery.value(0).toInt();	
		nStart = trialsQuery.value(1).toDouble();
		nEnd = trialsQuery.value(2).toDouble();
		bStart = trialsQuery.value(3).toDouble();
		bEnd = trialsQuery.value(4).toDouble();
	

		startjitter = bStart - (coeff_.A + coeff_.B*nStart);
		endjitter = bEnd - (coeff_.A + coeff_.B*nEnd);

		trialsInsertQuery.prepare("UPDATE trials"
			"SET startjitter=:startjitter endjitter=:endjitter "
			"WHERE id=:id");
		trialsInsertQuery.bindValue(":startjitter",startjitter);
		trialsInsertQuery.bindValue(":endjitter",endjitter);
		trialsInsertQuery.bindValue(":id",id);

	}

	//set the correlation for each row as well (we can do this with a signle query
	trialsQuery.prepare("UPDATE trials SET correlation=:corr");
	trialsQuery.bindValue(":corr",coeff_.corr);
	trialsQuery.exec();
}

/*!	\Brief Updates alignemnt with any new (unmatched) trials.
 *
 *	This runs through the unmatched trials in the trial tables, and tries
 *	to find new matches.  If a match is found, then the alignemnt is updated.
 *	This is a quick function, and should be called whenever a new trial is
 *	added to either the behavioral or neural trial tables.
 *
 *	There are a number of assumptions made in the alignment that we need to be aware of:
 *		1. Every trial should generate a start/stop pair in both the neural and behavioral
 *		   tables
 *		2. Trials are added more or less in order (we use the id to determine the order
 *		   they were added).
 *		3. Trial marking started at more or less the same time for the neural and behavioral
 *		   trials (e.g. we didn't mark 200 neural trials, and then suddenly start marking 
 *		   behavioral trials).  The system can handle some variation here, but not too much
 *
 *	I'm using a different scheme for marking matched trials internally:
 *		0  - unmatched and unchecked
 *		1  - matched
 *		-1 - unmatched, but checked
 *	Upon returning from the function, all tables are remarked with matched(1) and
 *	unmatched(0).
 */
void AlignmentTool::doIncrementalAlignment()
{
	QSqlQuery neuralQuery(sessionDb_);
	QSqlQuery behavioralQuery(sessionDb_);
	QSqlQuery trialsQuery(sessionDb_);

	while(true)
	{
		neuralQuery.exec("SELECT id,timestamp,aligncode FROM neuraltrials WHERE matched = 0");

		//stop the loop when we're out of trials that haven't been checked.
		if(!neuralQuery.next())
			break;

		AlignmentEvent neuralStartEvent;
		neuralStartEvent.alignCode = neuralQuery.value(2).toInt();
		neuralStartEvent.timestamp = neuralQuery.value(1).toDouble();
		neuralStartEvent.id = neuralQuery.value(0).toInt();

		//This could be a start or end, so find it's matching pair within +/- 10 trials
		neuralQuery.prepare("SELECT id,timestamp,aligncode "
			"FROM neuraltrials "
			"WHERE id>:minID AND id<:maxId AND aligncode = :aligncode AND matched=0");
		neuralQuery.bindValue(":minID",neuralStartEvent.id-10);
		neuralQuery.bindValue(":maxID",neuralStartEvent.id+10);
		neuralQuery.bindValue(":aligncode",neuralStartEvent.alignCode);
		neuralQuery.exec();

		//if we don't find a match, mark the first event as unmatched(-1)
		//and go back to the beginning
		if(!neuralQuery.next())
		{
			neuralQuery.prepare("UPDATE neuraltrials SET matched=-1 WHERE id=:id");
			neuralQuery.bindValue(":id", neuralStartEvent.id);
			neuralQuery.exec();
			continue;
		}

		AlignmentEvent neuralEndEvent;
		neuralEndEvent.alignCode = neuralQuery.value(2).toInt();
		neuralEndEvent.timestamp = neuralQuery.value(1).toDouble();
		neuralEndEvent.id = neuralQuery.value(0).toInt();

		//We may need to flip the start/end values
		if(neuralStartEvent.timestamp > neuralEndEvent.timestamp)
		{
			AlignmentEvent temp;
			temp = neuralStartEvent;
			neuralStartEvent = neuralEndEvent;
			neuralEndEvent = temp;
		}

		//Find a start/stop pair in the behavioral trials table with ids within +/- 50
		//events of the neural start event.
		//If more than 2 matching events are found, we continue to narrow the search
		//range until only two events are found.
		int idRange = 50;
		do
		{
			behavioralQuery.prepare("SELECT id,timestamp,aligncode,trialnumber "
									"FROM behavioraltrials "
									"WHERE id>:MINid AND id< :MAXid AND aligncode=:aligncode AND matched = 0");
			behavioralQuery.bindValue(":MINid",neuralStartEvent.id-idRange);
			behavioralQuery.bindValue(":MAXid",neuralStartEvent.id+idRange);
			behavioralQuery.bindValue(":aligncode", neuralStartEvent.alignCode);
			behavioralQuery.exec();

			idRange--;
		}
		while(behavioralQuery.size()>2);

		//If we didn't find exactly two matching events
		//mark the neural events as unmatched(-1)
		if(behavioralQuery.size() != 2)
		{
			neuralQuery.prepare("UPDATE neuraltrials SET matched=-1 WHERE id=:id");
			neuralQuery.bindValue(":id", neuralStartEvent.id);
			neuralQuery.exec();
			neuralQuery.prepare("UPDATE neuraltrials SET matched=-1 WHERE id=:id");
			neuralQuery.bindValue(":id", neuralEndEvent.id);
			neuralQuery.exec();
			continue;

		}

		behavioralQuery.next();
		AlignmentEvent behavioralStartEvent;
		behavioralStartEvent.id = behavioralQuery.value(0).toInt();
		behavioralStartEvent.alignCode = behavioralQuery.value(1).toInt();
		behavioralStartEvent.timestamp = behavioralQuery.value(2).toDouble();
		behavioralStartEvent.trialNum = behavioralQuery.value(3).toInt();
		
		behavioralQuery.next();
		AlignmentEvent behavioralEndEvent;
		behavioralEndEvent.id = behavioralQuery.value(0).toInt();
		behavioralEndEvent.alignCode = behavioralQuery.value(1).toInt();
		behavioralEndEvent.timestamp = behavioralQuery.value(2).toDouble();
		behavioralEndEvent.trialNum = behavioralQuery.value(3).toInt();

		//if the trial numbers don't match, mark everything as unmatched(-1)
		if(behavioralStartEvent.trialNum != behavioralEndEvent.trialNum)
		{
			neuralQuery.prepare("UPDATE neuraltrials SET matched=-1 WHERE id=:id");
			neuralQuery.bindValue(":id", neuralStartEvent.id);
			neuralQuery.exec();
			neuralQuery.prepare("UPDATE neuraltrials SET matched=-1 WHERE id=:id");
			neuralQuery.bindValue(":id", neuralEndEvent.id);
			neuralQuery.exec();
			behavioralQuery.prepare("UPDATE behavioraltrials SET matched=-1 WHERE id=:id");
			behavioralQuery.bindValue(":id", behavioralStartEvent.id);
			behavioralQuery.exec();
			behavioralQuery.prepare("UPDATE behavioraltrials SET matched=-1 WHERE id=:id");
			behavioralQuery.bindValue(":id", behavioralEndEvent.id);
			behavioralQuery.exec();
			continue;
		}


		//We may need to flip the start/end values
		if(behavioralStartEvent.timestamp > behavioralEndEvent.timestamp)
		{
			AlignmentEvent temp;
			temp = behavioralStartEvent;
			behavioralStartEvent = behavioralEndEvent;
			behavioralEndEvent = temp;
		}

		//Mark all four events as matched
		neuralQuery.prepare("UPDATE neuraltrials SET matched=1 WHERE id=:id");
		neuralQuery.bindValue(":id", neuralStartEvent.id);
		neuralQuery.exec();
		neuralQuery.prepare("UPDATE neuraltrials SET matched=1 WHERE id=:id");
		neuralQuery.bindValue(":id", neuralEndEvent.id);
		neuralQuery.exec();
		behavioralQuery.prepare("UPDATE behavioraltrials SET matched=1 WHERE id=:id");
		behavioralQuery.bindValue(":id", behavioralStartEvent.id);
		behavioralQuery.exec();
		behavioralQuery.prepare("UPDATE behavioraltrials SET matched=1 WHERE id=:id");
		behavioralQuery.bindValue(":id", behavioralEndEvent.id);
		behavioralQuery.exec();

		//update the coefficients
		updateCoefficients(behavioralStartEvent,behavioralEndEvent,neuralStartEvent,neuralEndEvent);

		//calculate jitter
		double startjitter,endjitter;

		startjitter = behavioralStartEvent.timestamp - (coeff_.A + coeff_.B*neuralStartEvent.timestamp);
		endjitter = behavioralEndEvent.timestamp - (coeff_.A + coeff_.B*neuralEndEvent.timestamp);

		//insert everything into the trials table
		trialsQuery.prepare("INSERT INTO trials(trialnumber,aligncode,neuralstart, "
							"neuralend,behavioralstart,behavioralend,"
							"startjitter,endjitter,correlation) "
							"VALUES :trialnumber,:aligncode,:neuralstart, "
							":neuralend,:behavioralstart,:behavioralend,"
							":startjitter,:endjitter,:corr");
		trialsQuery.bindValue(":trialnumber",behavioralStartEvent.trialNum);
		trialsQuery.bindValue(":aligncode",behavioralStartEvent.alignCode);
		trialsQuery.bindValue(":neuralstart",neuralStartEvent.timestamp);
		trialsQuery.bindValue(":neuralend",neuralEndEvent.timestamp);
		trialsQuery.bindValue(":behavioralstart",behavioralStartEvent.timestamp);
		trialsQuery.bindValue(":behavioralend",behavioralEndEvent.timestamp);
		trialsQuery.bindValue(":startjitter",startjitter);
		trialsQuery.bindValue(":endjitter",endjitter);
		trialsQuery.bindValue(":corr",coeff_.corr);
		trialsQuery.exec();

	}

	//return all of the matched=-1 values back to matched=0
	neuralQuery.exec("UPDATE neuraltrials SET matched=0 WHERE matched=-1");
}


/*!	\brief Updates the coefficients when a new complete trial is found.
 *
 *	The function updates the running sums, and then uses those to 
 *	calculate the newest coefficients.
 *
 *	The math is based on:
 *	  http://mathworld.wolfram.com/LeastSquaresFitting.html
 *	
 *	Note that the neural timebase is the x-axis, while the behavioral
 *	timebase is the y-axis
 */
void AlignmentTool::updateCoefficients(AlignmentEvent bStartEvent,
									   AlignmentEvent bEndEvent,
									   AlignmentEvent nStartEvent,
									   AlignmentEvent nEndEvent)

{
	//update the sums
	n_ += 2;
	sumXX_ += nStartEvent.timestamp*nStartEvent.timestamp;
	sumXX_ += nEndEvent.timestamp*nEndEvent.timestamp;
	sumYY_ += bStartEvent.timestamp*bStartEvent.timestamp;
	sumYY_ += bEndEvent.timestamp*bEndEvent.timestamp;
	sumXY_ += nStartEvent.timestamp*bStartEvent.timestamp;
	sumXY_ += nEndEvent.timestamp*bEndEvent.timestamp;
	sumX_ += nStartEvent.timestamp + nEndEvent.timestamp;
	sumY_ += bStartEvent.timestamp + bEndEvent.timestamp;

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

	coeff_.B = SSxy/SSxx;
	coeff_.A = meanY - coeff_.B*meanX;
	coeff_.corr = (SSxy*SSxy)/(SSxx*SSyy);

}