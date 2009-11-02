#include <qDebug>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>

#include "alignmenttool.h"

AlignmentTool::AlignmentTool(QSqlDatabase& sessionDb)
	:sessionDb(sessionDb)
{
	//set up the sums
	sumXX = 0.0;
	sumYY = 0.0;
	sumXY = 0.0;
	sumX = 0.0;
	sumY = 0.0;
	n = 0;

	//set up an initial fit of y=x with no correlation
	coeff.A = 0.0;
	coeff.B = 1.0;  
	coeff.corr = 0.0;

	trials = 0; 
	orphanedCodes = 0;

	qDebug()<<"!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!";
	qDebug()<<"The alignment tool has not been tested (due to the difficulty of creating a test program)";
	qDebug()<<"Do not use this code until it has been tested";

}
AlignmentTool::~AlignmentTool()
{
}

Align::coefficients AlignmentTool::getCoefficients()
{
	return coeff;
}

void AlignmentTool::addEvent(int alignCode, double timestamp, Align::EventType eventType)
{
	alignmentEvent newEvent;
	newEvent.timestamp = timestamp;
	newEvent.alignCode = alignCode;
	newEvent.age = 0;

	//add the event to the appropriate list
	if(eventType == Align::behavioral)
	{
		behavioralEvents.push_back(newEvent);
	}
	else if(eventType == Align::neural)
	{
		neuralEvents.push_back(newEvent);
	}
	else
	{
		qDebug()<<"aligntool: Invalid eventType";
	}

	int bStartIdx, bEndIdx, nStartIdx, nEndIdx;
	if(completedTrial(bStartIdx, bEndIdx, nStartIdx, nEndIdx))
	{
		updateCoefficients(bStartIdx, bEndIdx, nStartIdx, nEndIdx);
		writeToDb(bStartIdx, bEndIdx, nStartIdx, nEndIdx);

		//finally, we'll remove the alignment codes from our list
		neuralEvents.removeAt(nStartIdx);
		neuralEvents.removeAt(nEndIdx);
		behavioralEvents.removeAt(bStartIdx);
		behavioralEvents.removeAt(bEndIdx);

		//check for excessive orphaned codes
		Q_ASSERT(trials<10 || (double)orphanedCodes/(trials*4.0) < 0.02);
	}
	ageTrialLists();
}


//completedTrial looks through the list of active events and tries 
//to match them up.  If a complete trial is found (a pair of alignment
//codes from both the neural data and the behavioral data), the function
//returns true, and places the indices in the passed in values.  Otherwise, 
//the function returns false, and the indices are unchanged.

//This function is called everytime an event is added.
bool AlignmentTool::completedTrial(int& bStartIdx, int& bEndIdx, 
								   int& nStartIdx, int& nEndIdx)
{
	//loop through all the neural events
	for(int n = 0; n < neuralEvents.size(); n++)
	{
		if(neuralEvents.count(neuralEvents[n]) == 2 &&
			behavioralEvents.count(neuralEvents[n]) == 2)
		{
			nStartIdx = n;
			nEndIdx = neuralEvents.indexOf(neuralEvents[nStartIdx],nStartIdx+1);
			bStartIdx = behavioralEvents.indexOf(neuralEvents[nStartIdx],0);
			bEndIdx = behavioralEvents.indexOf(neuralEvents[nStartIdx],bStartIdx + 1);

			//check that everything is in order
			if(neuralEvents[nStartIdx].timestamp > neuralEvents[nEndIdx].timestamp)
			{
				int tempIdx;
				tempIdx = nStartIdx;
				nStartIdx = nEndIdx;
				nEndIdx = tempIdx;
			}
			if(behavioralEvents[bStartIdx].timestamp > behavioralEvents[bEndIdx].timestamp)
			{
				int tempIdx;
				tempIdx = bStartIdx;
				bStartIdx = bEndIdx;
				bEndIdx = tempIdx;
			}

			trials += 4;

			return true;
		}
		else if(neuralEvents.count(neuralEvents[n]) > 2)
		{
			QSqlQuery query(sessionDb);
			//If there are more than 2 identical codes in the list, we'll 
			//simply dump all of the codes into the orphan table, and remove them from our list
			int deleteIdx = neuralEvents.indexOf(neuralEvents[n]);
			while (deleteIdx != -1)
			{
				query.prepare("INSERT INTO orphantrials (aligncode,timestamp,source) "
					"VALUES (:aligncode, :timestamp, 'neural')");
				query.bindValue(":aligncode",neuralEvents[deleteIdx].alignCode);
				query.bindValue(":timestamp",neuralEvents[deleteIdx].timestamp);
				Q_ASSERT(query.exec());
				
				neuralEvents.removeAt(deleteIdx);
				orphanedCodes ++;

				deleteIdx = neuralEvents.indexOf(neuralEvents[n]);
			}

			qDebug()<<"Too many neural events with the same aligncodes!";
		}
		else if(behavioralEvents.count(behavioralEvents[n]) > 2)
		{
			QSqlQuery query(sessionDb);
			//If there are more than 2 identical codes in the list, we'll 
			//simply dump all of the codes into the orphan table, and remove them from our list
			int deleteIdx = behavioralEvents.indexOf(behavioralEvents[n]);
			while (deleteIdx != -1)
			{
				query.prepare("INSERT INTO orphantrials (aligncode,timestamp,source) "
					"VALUES (:aligncode, :timestamp, 'behavioral')");
				query.bindValue(":aligncode",behavioralEvents[deleteIdx].alignCode);
				query.bindValue(":timestamp",behavioralEvents[deleteIdx].timestamp);
				Q_ASSERT(query.exec());
				
				behavioralEvents.removeAt(deleteIdx);
				orphanedCodes ++;

				deleteIdx = behavioralEvents.indexOf(neuralEvents[n]);
			}

			qDebug()<<"Too many neural events with the same aligncodes!";
		}
	}
	return false;


}

//The events should be coming in at approximately the 
//same times (give or take a bit), so alignment should be easy.  However, 
//there remains a possibility of codes failing to align (for example, if
//behavioral codes start coming in before neural codes, or visa versa).  
//If this occurs, it could result in an overflow of the lists.  To prevent this,
//all events are stamped with an age that increases every time a new event is
//added.  Since 4 events are added for each trial, we will then delete any events
// that are older than 16 (4 trials worth of events).

//Additionally, the old events are sent to the orphantrials table so they can be 
//sorted out later..
void AlignmentTool::ageTrialLists()
{
	QSqlQuery query(sessionDb);
	for(int i=0; i<behavioralEvents.size(); i++)
	{
		if(behavioralEvents[i].age>16)
		{
			query.prepare("INSERT INTO orphantrials (aligncode,timestamp,source) "
				"VALUES (:aligncode, :timestamp, 'behavioral')");
			query.bindValue(":aligncode",behavioralEvents[i].alignCode);
			query.bindValue(":timestamp",behavioralEvents[i].timestamp);
			Q_ASSERT(query.exec());

			orphanedCodes++;
			behavioralEvents.removeAt(i);
		}
		behavioralEvents[i].age++;
	}
	for(int i=0; i<neuralEvents.size(); i++)
	{
		if(neuralEvents[i].age>16)
		{
			query.prepare("INSERT INTO orphantrials (aligncode,timestamp,source) "
				"VALUES (:aligncode, :timestamp, 'neural')");
			query.bindValue(":aligncode",neuralEvents[i].alignCode);
			query.bindValue(":timestamp",neuralEvents[i].timestamp);
			Q_ASSERT(query.exec());
			
			orphanedCodes++;
			neuralEvents.removeAt(i);
		}
		neuralEvents[i].age++;
	}

	return;
}



//updateCoefficients is called whenever a complete trial is found.
//The function updates the running sums, and then uses those to 
//calculate the newest coefficients.

////The math is based on:
		//  http://mathworld.wolfram.com/LeastSquaresFitting.html
void AlignmentTool::updateCoefficients(int bStartIdx, int bEndIdx, 
								   int nStartIdx, int nEndIdx)
{
	//update the sums
	n += 2;
	sumXX += neuralEvents[nStartIdx].timestamp*neuralEvents[nStartIdx].timestamp;
	sumXX += neuralEvents[nEndIdx].timestamp*neuralEvents[nEndIdx].timestamp;
	sumYY += behavioralEvents[bStartIdx].timestamp*behavioralEvents[bStartIdx].timestamp;
	sumYY += behavioralEvents[bEndIdx].timestamp*behavioralEvents[bEndIdx].timestamp;
	sumXY += neuralEvents[nStartIdx].timestamp*behavioralEvents[bStartIdx].timestamp;
	sumXY += neuralEvents[nEndIdx].timestamp*behavioralEvents[bEndIdx].timestamp;
	sumX += neuralEvents[nStartIdx].timestamp + neuralEvents[nEndIdx].timestamp;
	sumY += neuralEvents[nStartIdx].timestamp + neuralEvents[nEndIdx].timestamp;

	//calulate the new coefficients
		//SSxx = sum(x[i]^2) - n*mean(x)^2
		//SSyy = sum(y[i]^2) - n*mean(y)^2
		//SSxy = sum(x[i]y[i]) - n*mean(x)*mean(y)
		
		//B = SSxy/SSxx
		//A = mean(y) - B*mean(x)
		//corr = r^2 = SSxy^2/(SSxx*SSyy)

	double meanX = sumX/n;
	double meanY = sumY/n;

	double SSxx = sumXX - n*meanX*meanX;
	double SSyy = sumYY - n*meanY*meanY;
	double SSxy = sumXY - n*meanX*meanY;

	coeff.B = SSxy/SSxx;
	coeff.A = meanY - coeff.B*meanX;
	coeff.corr = (SSxy*SSxy)/(SSxx*SSyy);

}

//writeToDb outputs the information about a completed trial to the session database
//most of the fields are obvious, however, the jitter fields deserve some
//discussion.  Since we can't possibly know the true relationship between the
//two timebases, to calculate jitter, we compare the fit values to the best fit
//line. Additionally, we output the current value of the correlation coefficient.
void AlignmentTool::writeToDb(int bStartIdx, int bEndIdx, int nStartIdx, int nEndIdx)
{
		/*sessionQ.exec("CREATE TABLE trials (trialnumber INTEGER PRIMARY KEY, "
			"aligncode INTEGER, neuralstart REAL, neuralend REAL, "
			"behavioralstart REAL, behavioralend REAL, startjitter REAL, "
			"endjitter REAL, correlation REAL)");*/

	int trialnumber;

	if(!sessionDb.tables().contains("trials"))
		return;

	QSqlQuery query(sessionDb);

	//figure out our trial number (this should be trivial, but there is a chance that
	//we are working out of order)
	query.exec("SELECT trialnumber aligncode FROM trials ORDER BY aligncode DESC");

	//first trial case
	if(query.size() == 0)
	{
		trialnumber = 1;
	}
	//not first trial
	else 
	{
		//grab the most recent trial, and use it to figure out what our trial number is
		//The math should work regardless of skipped trials)
		query.next();
		trialnumber = query.value(0).toInt() + 
			(neuralEvents[nStartIdx].alignCode - query.value(1).toInt());
	}

	//just to be safe, confirm that the alignment code/trial number pair
	//doesn't already exist in the database.
	query.prepare("SELECT FROM trials WHERE aligncode = :aligncode OR trialnumber = :trialnumber");
	query.bindValue(":aligncode", neuralEvents[nStartIdx].alignCode);
	query.bindValue(":trialnumber",trialnumber);
	query.exec();

	if(query.next())
	{
		qDebug()<<"Two trials with the same trial number or alignment code!";
	}

	//find the jitter values
	double startjitter = neuralEvents[nStartIdx].timestamp - 
		(coeff.A + coeff.B*behavioralEvents[bStartIdx].timestamp);
	double endjitter = neuralEvents[nStartIdx].timestamp - 
		(coeff.A + coeff.B*behavioralEvents[bStartIdx].timestamp);

	//insert data into the trials table
	query.prepare("INSERT INTO trials(trialnumber,aligncode,neuralstart, "
		"neuralend,behavioralstart,behavioralend,startjitter,endjitter,correlation) "
		"VALUES :trialnumber,:aligncode,:neuralstart, "
		":neuralend,:behavioralstart,:behavioralend,:startjitter,:endjitter,:correlation");
	query.bindValue(":trialnumber",trialnumber);
	query.bindValue(":aligncode",neuralEvents[nStartIdx].alignCode);
	query.bindValue(":neuralstart",neuralEvents[nStartIdx].timestamp);
	query.bindValue(":neuralend",neuralEvents[nEndIdx].timestamp);
	query.bindValue(":behavioralstart",behavioralEvents[bStartIdx].timestamp);
	query.bindValue(":behavioralend",behavioralEvents[bEndIdx].timestamp);
	query.bindValue(":startjitter",startjitter);
	query.bindValue(":endjitter",endjitter);
	query.bindValue(":correlation",coeff.corr);
	Q_ASSERT(query.exec());
}


