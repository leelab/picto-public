#ifndef _TASK_RUN_VIEWER_H_
#define _TASK_RUN_VIEWER_H_

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSharedPointer>
#include "../../common/storage/TaskRunDataUnit.h"

/*!	\brief	This views and session data including information about the current tasks run.
 * 
 */
class TaskRunViewer : public QWidget
{
	Q_OBJECT
public:
	TaskRunViewer(QWidget *parent=0);
	virtual ~TaskRunViewer();
	void setTaskRunData(QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>> dataMap);
	void markLatestAsRunning(bool isRunning);
	void clear();
signals:
	void taskRunDataChanged(qulonglong runId);
private:
	QSharedPointer<Picto::TaskRunDataUnit> getCurrentRun();
	void updateComboBox();
	void updateFieldsFromCurrentRun();
	void updateCurrentRunFromFields();
	void setStateToEditing();
	void setStateToReadOnly();
	QVBoxLayout *mainLayout_;
	QComboBox *availableRuns_;
	QLineEdit *runName_;
	QTextEdit *runNotes_;
	QToolButton *runSaved_;
	QPushButton *cancelButton_;
	QPushButton *actButton_;
	QHBoxLayout *buttonLayout_;
	QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>> currTaskRuns_;
	bool editing_;
	bool latestIsRunning_;
private slots:
	void actTriggered();
	void cancelTriggered();
	void availableRunsIndexChanged(int index);
	void saveToggled(bool checked);

};

#endif