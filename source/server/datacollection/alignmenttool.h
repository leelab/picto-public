#ifndef ALIGNMENTTOOL_H
#define ALIGNMENTTOOL_H

#include <QList>
#include <QSqlDatabase>

namespace Align{
	//Y = A + BX
	//corr = correlation coefficient
	typedef struct 
	{
		double A;
		double B;
		double corr;
	} coefficients;

	typedef enum
	{
		neural,
		behavioral
	} EventType;
};

class AlignmentTool
{
public:
	AlignmentTool(QSqlDatabase& sessionDb);
	~AlignmentTool();

	Align::coefficients getCoefficients();
	void addEvent(int alignCode, double timestamp, Align::EventType eventType);

private:
	void updateCoefficients(int bStartIdx, int bEndIdx, int nStartIdx, int nEndIdx);
	bool completedTrial(int& bStartIdx, int& bEndIdx, int& nStartIdx, int& nEndIdx);
	void ageTrialLists();
	void writeToDb(int bStartIdx, int bEndIdx, int nStartIdx, int nEndIdx);

	QSqlDatabase sessionDb;

	struct alignmentEvent
	{
		int alignCode;
		double timestamp;
		int age;
		bool operator==(const alignmentEvent &rhs)
		{
			return (this->alignCode == rhs.alignCode);
		};
	};
	QList<alignmentEvent> behavioralEvents, neuralEvents;
	Align::coefficients coeff;

	//Fitting values
	//SumXX = sum(x[i]^2)
	//SumYY = sum(y[i]^2)
	//SumXY = sum(x[i]y[i])
	//SumX = sum(x[i])  used to calculate mean(x)
	//SumY = sum(y[i])  used to calculate mean(y)
	//n = number of points in fit
	double sumXX, sumYY, sumXY;
	double sumX, sumY;
	int n;

	//statistics
	int trials;
	int orphanedCodes;


};


#endif

