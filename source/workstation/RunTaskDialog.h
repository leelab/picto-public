#ifndef _RUN_TASK_DIALOG_H_
#define _RUN_TASK_DIALOG_H_

#include <QDialog>
#include <QUuid>

#include "../common/network/CommandChannel.h"
#include "../common/experiment/experiment.h"

class QComboBox;
class QLabel;
class QDialogButtonBox;

/*!	\brief Deprecated. A dialog for running a task.
 *
 *	This was originally used as a dialog to start up a new task.  We no longer use 
 *	this dialog, and hence the code is no longer needed.  However, I have left it in place
 *	as it shows exactly how to run a task.
 */
class RunTaskDialog : public QDialog
{
	Q_OBJECT
public:
	RunTaskDialog(QSharedPointer<Picto::CommandChannel> commandChannel, QSharedPointer<Picto::Experiment> experiment, QUuid sessionId);
	virtual ~RunTaskDialog(){};

private slots:
	void runTask();

private:
	void createDialog();

	QLabel *taskLabel_;
	QComboBox *taskComboBox_;
	QDialogButtonBox *buttonBox_;

	QSharedPointer<Picto::CommandChannel> commandChannel_;
	QWeakPointer<Picto::Experiment> experiment_;
	QUuid sessionId_;
};

#endif