#ifndef _RUN_TASK_DIALOG_H_
#define _RUN_TASK_DIALOG_H_

#include <QDialog>
#include <QUuid>

#include "../common/network/CommandChannel.h"
#include "../common/experiment/experiment.h"

class QComboBox;
class QLabel;
class QDialogButtonBox;

class RunTaskDialog : public QDialog
{
	Q_OBJECT
public:
	RunTaskDialog(QSharedPointer<Picto::CommandChannel> commandChannel, QSharedPointer<Picto::Experiment> experiment, QUuid sessionId);

private slots:
	void runTask();

private:
	void createDialog();

	QLabel *taskLabel_;
	QComboBox *taskComboBox_;
	QDialogButtonBox *buttonBox_;

	QSharedPointer<Picto::CommandChannel> commandChannel_;
	QSharedPointer<Picto::Experiment> experiment_;
	QUuid sessionId_;
};

#endif