#ifndef _BEHAVVIEWER_H_
#define _BEHAVVIEWER_H_

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
class BehavViewer : public QWidget
{
	Q_OBJECT
public:
	BehavViewer(QWidget *parent=0);
	virtual ~BehavViewer();
	void setTaskRunData(QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>> dataMap);
	void markLatestAsRunning(bool isRunning);
	QIcon getLatestRunIcon();
	void enableEditing(bool enable);
	void clear();
signals:
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