#ifndef ALIGNMENTTOOL_H
#define ALIGNMENTTOOL_H

#include <QString>
#include <QSqlDatabase>

class AlignmentTool
{
public:
	AlignmentTool();
	bool mergeDatabases(QString neuralDBFilename, QString behavioralDBFilename, QString mergedDBFilename);
	QString lastError() { return errorString; };
private:
	bool openDBs(QString neuralDBFilename, QString behavioralDBFilename, QString mergedDBFilename);
	void closeDBs();
	void initMergedDB();
	bool processAlignmentTable(QSqlDatabase *db, QString tablename);
	bool align();
	void linearLeastSquares(const QList<double> &xData, const QList<double> &yData, int xIndex, int yIndex, double &a, double &b, double &corr);

	QSqlDatabase neuralDB;
	QSqlDatabase behavioralDB;
	QSqlDatabase mergedDB;

	QString errorString;
};



#endif