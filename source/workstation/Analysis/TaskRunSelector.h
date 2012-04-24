#ifndef _TASK_RUN_SELECTOR_H_
#define _TASK_RUN_SELECTOR_H_

#include <QSqlDatabase>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QSharedPointer>
#include <QListWidgetItem>
#include <QBrush>
#include "../../common/storage/TaskRunDataUnit.h"

/*!	\brief	Widget for selecting TaskRuns from a session.
 */
class TaskRunSelector : public QWidget
{
	Q_OBJECT
public:
	TaskRunSelector(QWidget *parent=0);
	virtual ~TaskRunSelector();
	void loadSession(QSqlDatabase session);
	int selectedRunCount();
	QSharedPointer<Picto::TaskRunDataUnit> getSelectedRun(int index);
	qulonglong startFrameOfSelectedRun(int index);
	qulonglong endFrameOfSelectedRun(int index);
	double startTimeOfSelectedRun(int index);
	double endTimeOfSelectedRun(int index);
	QString nameOfSelectedRun(int index);

private:
	struct RunData
	{
		RunData(QSharedPointer<Picto::TaskRunDataUnit> runUnit,double tStart,double tEnd){run=runUnit;startTime=tStart;endTime=tEnd;};
		QSharedPointer<Picto::TaskRunDataUnit> run;
		double startTime;
		double endTime;
	};
	RunData getRunDataFromSelectedIndex(int selIndex);
	QLabel* title_;
	QListWidget* selectArea_;
	QPushButton* clear_;
	QPushButton* selectAll_;
	QSqlDatabase session_;
	QList<RunData> runs_;

private slots:
	void clearSelections();
	void selectAll();
};

#endif