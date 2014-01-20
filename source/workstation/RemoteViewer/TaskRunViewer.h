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

/*!	\brief A viewer that displays data about the Task Runs from the current session and allows the operator to edit them.
 *	\details This viewer allows the Experiment operator to save notes about the runs in the current session, set their names
 *	and set a saved flag indicating if they contain useful data.  It communicates with the outside world by accessing run
 *	data through input pointers and changing run data directly in the objects that those pointers point to.  When the
 *	taskRunDataChanged() signal is emitted, the outside world knows that something changed in the Task Run that it 
 *	indicates and that data can be sent to the Picto Server.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class TaskRunViewer : public QWidget
{
	Q_OBJECT
public:
	TaskRunViewer(QWidget *parent=0);
	virtual ~TaskRunViewer();
	void setTaskRunData(QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>> dataMap);
	void markLatestAsRunning(bool isRunning);
	QIcon getLatestRunIcon();
	void enableEditing(bool enable);
	void clear();
signals:
	/*! \brief Emitted when the widget is used to change values in the Run with the input runId.  When this is emitted, the
	 *	latest values of the run with that runId should be sent to the server.
	 *	\note The TaskRunViewer operates on the objects pointed to in the QMap passed into setTaskRunData().  When this signal
	 *	is emitted, those object will have already been updated.
	 */
	void taskRunDataChanged(qulonglong runId);
private:
	QSharedPointer<Picto::TaskRunDataUnit> getSelectedRun();
	QSharedPointer<Picto::TaskRunDataUnit> getLatestRun();
	bool selectedRunIsActive();
	void updateComboBox();
	void updateFieldsFromCurrentRun();
	void updateCurrentRunFromFields();
	void updateSaveButtonIcon();
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
	bool editingEnabled_;
private slots:
	void actTriggered();
	void cancelTriggered();
	void availableRunsIndexChanged(int index);
	void saveToggled(bool checked);

};

#endif